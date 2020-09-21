#ifndef CORE_TABLE_H
#define CORE_TABLE_H

#include <vector>

#include "core/assert.h"
#include "core/numeric.h"
#include "core/rect.h"
#include "core/cint.h"

namespace euphoria::core
{
    template <typename T>
    struct Table
    {
        using I = int;

        [[nodiscard]] static constexpr I
        Ci(size_t t)
        {
            return Csizet_to_int(t);
        }

        Table() : width(0), height(0) {}

        [[nodiscard]] static Table
        FromWidthHeight(I width, I height, T d = T())
        {
            return Table(width, height, d);
        }

        [[nodiscard]] static Table
        FromHeightWidth(I height, I width, T d = T())
        {
            return Table(width, height, d);
        }

        void
        NewRow(T d = T())
        {
            ASSERT(width > 0);
            data.resize(data.size() + width, d);
            height += 1;
        }

        void
        Clear(T d = T())
        {
            for(size_t i = 0; i < data.size(); i += 1)
            {
                data[i] = d;
            }
        }

        template <typename Func>
        void
        SetAll(Func f)
        {
            for(I y = 0; y < height; y += 1)
            {
                for(I x = 0; x < width; x += 1)
                {
                    (*this)(x, y) = f(x, y);
                }
            }
        }

        const Recti
        Indices() const
        {
            return Recti::FromWidthHeight(width - 1, height - 1);
        }

        bool
        IsInside(I x, I y) const
        {
            return Indices().ContainsInclusive(x, y);
        }

        /** Add a new row to the table.
         * Places the new row at the bottom left and resizes the table if necessary.
         * Fills the (new) empty space with default values.
         * @param row the new row
         * @param d the default value (if needed)
         */
        void
        NewRow(const std::vector<T>& row, T d = T())
        {
            // do nothing on empty rows
            if(row.empty())
                return;

            if(width == 0)
            {
                // this is a new table, the row is the new table
                ASSERTX(height == 0, height);
                data   = row;
                width  = row.size();
                height = 1;
            }
            else if(width == Ci(row.size()))
            {
                // the number of columns math the new new, just insert it
                data.insert(data.end(), row.begin(), row.end());
                height += 1;
            }
            else if(width > Ci(row.size()))
            {
                // new row has less columns than the table, solve by expanding the new row with empties
                data.insert(data.end(), row.begin(), row.end());
                for(auto i = Ci(row.size()); i < width; i += 1)
                {
                    data.push_back(d);
                }
                height += 1;
            }
            else
            {
                // new row has more columns, solve by expanding table with empties
                ASSERTX(width < Ci(row.size()), width, row.size());
                ExpandWidth(row.size(), d);
                data.insert(data.end(), row.begin(), row.end());
                height += 1;
            }
        }

        /** Expand the table to the new width.
         * Aligns existing content to the left, fills the empty space with default values.
         * @param new_width the new width
         * @param d the default value
         */
        void
        ExpandWidth(I new_width, T d = T())
        {
            // todo: only insert the columns we need
            ASSERT(width != 0);
            ASSERTX(width < new_width, width, new_width);

            auto t = Table<T>::FromWidthHeight(new_width, height, d);
            for(I x = 0; x < width; x += 1)
            {
                for(I y = 0; y < height; y += 1)
                {
                    t(x, y) = (*this)(x, y);
                }
            }
            *this = t;
        }

        // NewCol function??? probably less useful than insert functions

        // todo: insert before/after row/col
        // todo: remove row/col


        T
        operator()(I x, I y) const
        {
            const auto index = DataIndex(x, y);
            ASSERTX(index < data.size(), index, data.size());
            return data[index];
        }

        typename std::vector<T>::reference
        operator()(I x, I y)
        {
            const auto index = DataIndex(x, y);
            ASSERTX(index < data.size(), index, data.size());
            return data[index];
        }

        // todo: figure out a better name
        I
        GetWidth() const
        {
            return width;
        }

        I
        GetHeight() const
        {
            return height;
        }

        size_t
        DataIndex(I x, I y) const
        {
            ASSERTX(IsWithinInclusivei(0, x, width - 1)
                            && IsWithinInclusivei(0, y, height - 1),
                    x,
                    width,
                    y,
                    height);
            return y * width + x;
        }

    private:
        Table(I c, I r, T d = T()) : data(c * r, d), width(c), height(r)
        {
            ASSERTX(width >= 0 && height >= 0, width, height);
        }

        // table is stored in in place like [row, row, ...row]
        std::vector<T> data;
        I              width;
        I              height;
    };

    template <typename T>
    std::vector<T>
    CalcColumnAsVector(const Table<T>& t, typename Table<T>::I x)
    {
        ASSERTX(x < t.GetWidth(), x, t.GetWidth());
        std::vector<T> r;
        r.reserve(t.GetHeight());
        for(typename Table<T>::I y = 0; y < t.GetHeight(); ++y)
        {
            r.emplace_back(t(x, y));
        }
        return r;
    }

    template <typename T>
    std::vector<T>
    CalcRowAsVector(const Table<T>& t, typename Table<T>::I y)
    {
        ASSERTX(y < t.GetHeight(), y, t.GetHeight());

        std::vector<T> r;
        r.reserve(t.GetWidth());
        for(typename Table<T>::I x = 0; x < t.GetWidth(); ++x)
        {
            r.emplace_back(t(x, y));
        }
        return r;
    }

}  // namespace euphoria::core

#endif  // CORE_STRING_TABLE_H