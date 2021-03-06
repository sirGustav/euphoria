#pragma once

#include "core/noncopyable.h"

#include "render/gltypes.h"


namespace euphoria::render
{
    struct id
    {
    public:
        id();
        ~id() = default;

        NONCOPYABLE(id);

        // todo(Gustav): remove and make id public or remove class altogether
        [[nodiscard]] gluint
        get_id() const;

    protected:
        // nolint regarding protected visibility
        gluint id_; // NOLINT
    };
}
