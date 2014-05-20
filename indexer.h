#ifndef INDEXER_H
#define INDEXER_H

#include <assert.h>

namespace Fabio {

template <size_t Dim>
struct indexer {
    typedef indexer<Dim> this_t;

    indexer() : shift(0) {
        for (size_t i = 0; i < Dim; ++ i)
            coeffs[i] = 0;
    }

    static this_t from_sizes(const size_t* s) {
        this_t ind;
        ind.coeffs[0] = 1;
        for (size_t i = 0; i < Dim - 1; ++i)
            ind.coeffs[i + 1] = ind.coeffs[i] * s[i];
        return ind;
    }
    
    static this_t from_sizes(size_t s0, size_t s1 = 0, size_t s2  = 0, size_t s3 = 0) {
        assert (Dim <= 5);
        const int s[4] = {s0, s1, s2, s3};
        return from_sizes(s);
    }

    indexer(const int *cfs, int sh = 0) : shift(sh) {
        for(size_t i = 0; i < Dim; ++i)
            coeffs[i] = cfs[i];
    }
    
    
                            
    indexer<Dim - 1> slice(size_t dim, int pos) const {
        indexer<Dim - 1> sl;
        sl.shift = shift + coeffs[dim] * pos;
        for(size_t i = 0; i < Dim - 1; ++i)
            if (i < dim)
                sl.coeffs[i] = coeffs[i];
            else
                sl.coeffs[i] = coeffs[i + 1];
        return sl;
    }
    int index (const int *indices) const {
        int res = shift;
        for (size_t i = 0; i < Dim; ++i)
            res += coeffs[i] * indices[i];
        return res;
    }
    int operator() (int i0, int i1 = 0, int i2 = 0, int i3 = 0) const {
        const int indices[4] = {i0, i1, i2, i3};
        return index(indices);
    }

    void indices(int* inds, int pos) const {
        int s = pos - shift;
        for (int i = Dim - 1; i >= 0; --i) {
            inds[i] = s / coeffs[i];
            s -= coeffs[i] * inds[i];
        }
    }
    int coeffs[Dim];
    int shift;
};

template<size_t Dim>
indexer<Dim> permute(const indexer<Dim>& ind, const size_t* dims)
{
    indexer<T, Dim> perm;
    for(size_t i = 0; i < Dim; ++i)
        perm.coeffs[i] = ind.coeffs[dims[i]];
    return perm;
};
    
}

#endif