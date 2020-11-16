#include "core/raytracer.h"

#include "core/image.h"

#include "core/intersection.h"
#include "core/sphere.h"
#include "core/random.h"
#include "core/polarcoord.h"
#include "core/numeric.h"
#include "core/cli_progress_bar.h"
#include "core/angle.h"

#include <limits>

namespace euphoria::core::raytracer
{
    HitResult::HitResult
    (
        float aray_distance,
        const vec3f& aposition,
        const unit3f& anormal,
        std::shared_ptr<Material> amaterial
    )
        : ray_distance(aray_distance)
        , position(aposition)
        , normal(anormal)
        , material(amaterial)
    {
    }


    struct SphereObject : public Object
    {
        Sphere sphere;
        vec3f position;
        std::shared_ptr<Material> material;

        SphereObject
        (
            const Sphere& asphere,
            const vec3f& aposition,
            std::shared_ptr<Material> amaterial
        )
            : sphere(asphere)
            , position(aposition)
            , material(amaterial)
        {
        }

        std::optional<HitResult>
        Hit(const UnitRay3f& ray, const Range<float>& range) const override
        {
            const auto hit_index = GetIntersection
            (
                ray,
                sphere,
                position
            );
            if(IsWithin(range, hit_index))
            {
                const auto hit_position = ray.GetPoint(hit_index);
                const auto hit_normal = vec3f::FromTo(position, hit_position).GetNormalized();
                return HitResult
                {
                    hit_index,
                    hit_position,
                    hit_normal,
                    material
                };
            }
            else
            {
                return std::nullopt;
            }
        }
    };


    std::shared_ptr<Object>
    CreateSphere
    (
        const Sphere& sphere,
        const vec3f& position,
        std::shared_ptr<Material> material
    )
    {
        return std::make_shared<SphereObject>
        (
            sphere,
            position,
            material
        );
    }


    std::optional<HitResult>
    Scene::Hit(const UnitRay3f& ray, const Range<float>& range) const
    {
        std::optional<HitResult> r = std::nullopt;

        for(const auto& o: objects)
        {
            const auto h = o->Hit(ray, range);
            if(r.has_value() == false)
            {
                r = h;
            }
            else
            {
                if(h.has_value())
                {
                    if(h->ray_distance < r->ray_distance)
                    {
                        r = h;
                    }
                }
            }
        }

        return r;
    }


    Rgb
    rgb(const unit3f& normal)
    {
        return Rgb
        {
            (normal.x + 1)/2,
            (normal.y + 1)/2,
            (normal.z + 1)/2
        };
    }


    vec3f RandomInUnitSphere(Random* random)
    {
        return RandomUnit3(random) * random->NextFloat01();
    }


    Material::~Material() {}

    struct DiffuseMaterial : public Material
    {
        Rgb albedo;

        explicit DiffuseMaterial(const Rgb& aalbedo)
            : albedo(aalbedo)
        {
        }

        std::optional<ScatterResult>
        Scatter
        (
            const UnitRay3f& /*ray*/,
            const HitResult& hit,
            Random* random
        ) override
        {
            const auto target = hit.position + hit.normal + RandomInUnitSphere(random);
            const auto reflected_ray = UnitRay3f::FromTo(hit.position, target);

            return ScatterResult
            {
                albedo,
                reflected_ray
            };
        }
    };


    vec3f Reflect(const vec3f& v, const unit3f& n) {
        return v - 2*dot(v,n)*n;
    }


    struct MetalMaterial : public Material
    {
        Rgb albedo;
        float fuzz;

        explicit MetalMaterial
        (
            const Rgb& aalbedo,
            float afuzz
        )
            : albedo(aalbedo)
            , fuzz(KeepWithin(R01(), afuzz))
        {
        }

        std::optional<ScatterResult>
        Scatter
        (
            const UnitRay3f& ray,
            const HitResult& hit,
            Random* random
        ) override
        {
            const auto reflected = Reflect
            (
                ray.dir,
                hit.normal
            );
            const auto scattered = UnitRay3f::FromTo
            (
                hit.position,
                reflected + fuzz * RandomInUnitSphere(random)
            );
            const auto scatter_dot = dot
            (
                scattered.dir,
                hit.normal
            );
            if(scatter_dot > 0)
            {
                return ScatterResult
                {
                    albedo,
                    scattered
                };
            }
            else
            {
                return std::nullopt;
            }
        }
    };


    std::optional<vec3f>
    Refract
    (
        const unit3f& uv,
        const unit3f& normal,
        float ni
    )
    {
        const auto dt = dot(uv, normal);
        const auto discriminant = 1.0f - ni*ni*(1.0f-dt*dt);
        if (discriminant > 0)
        {
            return ni * (uv - normal * dt) - normal * Sqrt(discriminant);
        }
        else
        {
            return std::nullopt;
        }
    }


    float
    FresnelFactor(float cosine, float ref_idx)
    {
        const auto r0 = Square
        (
            (1-ref_idx) / (1+ref_idx)
        );
        return r0 + (1-r0)*pow((1 - cosine), 5);
    }


    struct DielectricMaterial : public Material
    {
        Rgb albedo;
        float refractive_index;

        explicit DielectricMaterial
        (
            const Rgb& aalbedo,
            float arefractive_index
        )
            : albedo(aalbedo)
            , refractive_index(arefractive_index)
        {
        }

        std::optional<ScatterResult>
        Scatter
        (
            const UnitRay3f& ray,
            const HitResult& hit,
            Random* random
        ) override
        {
            const auto dr = dot(ray.dir, hit.normal);
            const auto dot_result = dr > 0.0f;

            const auto outward_normal = dot_result ? -hit.normal : hit.normal;
            const auto ni = dot_result ? refractive_index : 1.0f/refractive_index;

            const auto refracted = Refract(ray.dir, outward_normal, ni);

            if(refracted.has_value())
            {
                const auto cosine = dot_result
                    ? refractive_index * dr
                    : -dr
                    ;
                const auto reflection_probability = FresnelFactor(cosine, refractive_index);
                if( random->NextFloat01() >= reflection_probability )
                {
                    return ScatterResult
                    {
                        albedo,
                        UnitRay3f::FromTo
                        (
                            hit.position,
                            refracted.value()
                        )
                    };
                }
            }
            const auto reflected = Reflect
            (
                ray.dir,
                hit.normal
            );
            return ScatterResult
            {
                albedo,
                UnitRay3f::FromTo
                (
                    hit.position,
                    reflected
                )
            };
        }
    };


    std::shared_ptr<Material>
    CreateDiffuseMaterial
    (
        const Rgb& albedo
    )
    {
        return std::make_shared<DiffuseMaterial>
        (
            albedo
        );
    }


    std::shared_ptr<Material>
    CreateMetalMaterial
    (
        const Rgb& albedo,
        float fuzz
    )
    {
        return std::make_shared<MetalMaterial>
        (
            albedo,
            fuzz
        );
    }


    std::shared_ptr<Material>
    CreateDielectricMaterial
    (
        const Rgb& albedo,
        float refractive_index
    )
    {
        return std::make_shared<DielectricMaterial>
        (
            albedo,
            refractive_index
        );
    }


    Rgb
    GetColor
    (
        const Scene& scene,
        const UnitRay3f& ray,
        Random* random,
        int depth
    )
    {
        const auto h = scene.Hit
        (
            ray,
            MakeRange(0.001f, std::numeric_limits<float>::max())
        );
        if(h.has_value())
        {
            if(depth < 50)
            {
                auto scatter = h->material->Scatter
                (
                    ray,
                    h.value(),
                    random
                );
                if(scatter.has_value())
                {
                    const auto c = GetColor
                    (
                        scene,
                        scatter->scattered,
                        random,
                        depth + 1
                    );
                    return scatter->attenuation * c;
                }
                else
                {
                    return Color::Black;
                }
            }
            else
            {
                return Color::Black;
            }
        }
        const auto t = (ray.dir.y+1)/2.0f;
        return RgbTransform::Transform
        (
            Rgb(1.0f, 1.0f, 1.0f),
            t,
            Rgb(0.5f, 0.7f, 1.0f)
        );
    }


    struct Camera
    {
        static Camera Create(const Angle& vfov, float aspect)
        {
            const auto half_height = Tan(vfov/2.0f);
            const auto half_width = aspect * half_height;

            const auto lower_left_corner = vec3f{-half_width, -half_height, -1.0};
            const auto horizontal = vec3f{2*half_width, 0.0f, 0.0f};
            const auto vertical = vec3f{0.0f, 2*half_height, 0.0f};
            const auto origin = vec3f{0.0f, 0.0f, 0.0f};
            
            return Camera
            {
                lower_left_corner,
                horizontal,
                vertical,
                origin
            };
        }

        vec3f lower_left_corner;
        vec3f horizontal;
        vec3f vertical;
        vec3f origin;

        UnitRay3f GetRay(float u, float v) const
        {
            return UnitRay3f::FromTo(origin, lower_left_corner + u*horizontal + v*vertical);
        }
    };


    Rgb
    Gamma2CorrectColor(Rgb color)
    {
        return {Sqrt(color.r), Sqrt(color.g), Sqrt(color.b)};
    }

    void
    Raytrace(Image* aimage, const Scene& scene, int number_of_samples)
    {
        Image& img = *aimage;

        auto random = Random{};
        const auto aspect_ratio = img.GetWidth() / static_cast<float>(img.GetHeight());
        const auto camera = Camera::Create(Angle::FromDegrees(90), aspect_ratio);

        {
        std::cout << "Rendering ";
        CliProgressBar progress;

        const int number_of_pixels = img.GetWidth() * img.GetHeight();

        for(int y=0; y<img.GetHeight(); y+=1)
        for(int x=0; x<img.GetWidth(); x+=1)
        {
            Rgb color = Color::Black;
            for(int sample = 0; sample < number_of_samples; sample += 1)
            {
                const auto u = (x + random.NextFloat01()) / static_cast<float>(img.GetWidth());
                const auto v = (y + random.NextFloat01()) / static_cast<float>(img.GetHeight());
                const auto ray = camera.GetRay(u, v);
                const auto sample_color = GetColor(scene, ray, &random, 0);
                color += sample_color;
            }
            color = color/number_of_samples;
            color = Gamma2CorrectColor(color);
            img.SetPixel(x,y, rgbi(color));
            progress.Update( (y*img.GetWidth() + x)/static_cast<float>(number_of_pixels) );
        }
        }
        std::cout << "Rendering done :)\n";
    }
}
