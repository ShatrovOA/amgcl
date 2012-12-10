#ifndef AMGCL_OPERATIONS_VIENNACL_HPP
#define AMGCL_OPERATIONS_VIENNACL_HPP

/*
The MIT License

Copyright (c) 2012 Denis Demidov <ddemidov@ksu.ru>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

/**
 * \file   operations_viennacl.hpp
 * \author Denis Demidov <ddemidov@ksu.ru>
 * \brief  Adaptors for ViennaCL types.
 */

#include <amgcl/spmat.hpp>
#include <viennacl/compressed_matrix.hpp>

namespace amgcl {

namespace sparse {

template <class spmat>
class viennacl_matrix_adapter {
    public:
        typedef typename sparse::matrix_index<spmat>::type index_type;
        typedef typename sparse::matrix_value<spmat>::type value_type;

        class const_iterator1;

        class const_iterator2 {
            public:
                bool operator!=(const const_iterator2 &it) const {
                    return pos != it.pos;
                }

                const const_iterator2& operator++() {
                    ++pos;
                    return *this;
                }

                index_type index1() const {
                    return row;
                }

                index_type index2() const {
                    return col[pos];
                }

                value_type operator*() const {
                    return val[pos];
                }
            private:
                const_iterator2(index_type row, index_type pos,
                        const index_type *col, const value_type *val)
                    : row(row), pos(pos), col(col), val(val)
                { }

                index_type row;
                index_type pos;
                const index_type *col;
                const value_type *val;

                friend class const_iterator1;
        };

        class const_iterator1 {
            public:
                bool operator!=(const const_iterator1 &it) const {
                    return pos != it.pos;
                }

                const const_iterator1& operator++() {
                    ++pos;
                    return *this;
                }

                index_type index1() const {
                    return pos;
                }

                const const_iterator2 begin() const {
                    return const_iterator2(pos, row[pos], col, val);
                }

                const const_iterator2 end() const {
                    return const_iterator2(pos, row[pos + 1], col, val);
                }
            private:
                const_iterator1(index_type pos,
                        const index_type *row,
                        const index_type *col,
                        const value_type *val
                        )
                    : pos(pos), row(row), col(col), val(val)
                { }

                index_type pos;
                const index_type *row;
                const index_type *col;
                const value_type *val;

                friend class viennacl_matrix_adapter;
        };

        viennacl_matrix_adapter(const spmat &A)
            : rows(sparse::matrix_rows(A)),
              cols(sparse::matrix_cols(A)),
              row(sparse::matrix_outer_index(A)),
              col(sparse::matrix_inner_index(A)),
              val(sparse::matrix_values(A))
        { }

        const_iterator1 begin1() const {
            return const_iterator1(0, row, col, val);
        }

        const_iterator1 end1() const {
            return const_iterator1(rows, row, col, val);
        }

        index_type size1() const {
            return rows;
        }

        index_type size2() const {
            return cols;
        }
    private:
        index_type rows;
        index_type cols;

        const index_type *row;
        const index_type *col;
        const value_type *val;
};

template <class spmat>
viennacl_matrix_adapter<spmat> viennacl_map(const spmat &A) {
    return viennacl_matrix_adapter<spmat>(A);
}

} // namespace sparse
} // namespace amgcl

#endif
