#ifndef MDARRAY_H
#define MDARRAY_H

#include "indexer.h"
#include <iostream>

namespace Fabio {

#ifdef _DEBUG
template <typename T>
struct sized_ptr {
    T *ptr;
    size_t size;
    sized_ptr(T *p, size_t s = 0) : ptr(p), size(s) {}
    T& operator [] (size_t i) const {
        assert(i < size);        
        return ptr[i];
    }
    operator sized_ptr<const T> () const {
        return sized_ptr<const T> (ptr, size);
    }
    //const T& operator [] (size_t i) const {
    //    assert(i < size);        
    //    return ptr[i];
    //}

};
#endif

template <typename T, int Dim>
struct mdarray {
    typedef mdarray<T, Dim> this_t;
#ifdef _DEBUG
    sized_ptr<T> data;
#else
    T *data;
#endif
    size_t sizes[Dim];
    indexer<Dim> indexer;
    
                        mdarray     () : data(0) {
        for (size_t i = 0; i < Dim; ++i)
            sizes[i] = 0;
    }
                        mdarray     (size_t s0, size_t s1 = 0, size_t s2  = 0, size_t s3 = 0) : data(0) {
        assert (Dim <= 4);
        const size_t s[4] = {s0, s1, s2, s3};
        indexer = Fabio::indexer<Dim>::from_sizes(s);
        init(s);
    }
                        mdarray     (T* dat, size_t s0, size_t s1 = 0, size_t s2  = 0, size_t s3 = 0) : data(dat) {
        assert (Dim <= 4);
        const size_t s[4] = {s0, s1, s2, s3};
#ifdef _DEBUG
        data.size = 1;
        for (size_t i = 0; i < Dim; ++i)
            data.size *= s[i];
#endif
        indexer = Fabio::indexer<Dim>::from_sizes(s);
        init(s);
    }
                        mdarray     (const size_t *s) : data(0), indexer(Fabio::indexer<Dim>::from_sizes(s)) {
        init(s);
    }
                        mdarray     (T* dat, const size_t *s) : data(dat), indexer(Fabio::indexer<Dim>::from_sizes(s)) {
        init(s);
    }
    
    T&                  element     (const int* indices) const {
        size_t ind = indexer.index(indices);
#ifdef _DEBUG
        for(size_t i = 0; i < Dim; ++i)
            assert(indices[i] < sizes[i]);
        assert(ind < data.size);
#endif
        return data[ind];
    }

    T&                  operator () (size_t s0, size_t s1 = 0, size_t s2  = 0, size_t s3 = 0) const {
        size_t ind = indexer(s0, s1, s2, s3);
#ifdef _DEBUG
        switch (Dim)
        {
        case 4:
            assert(s3 < sizes[3]);
        case 3:
            assert(s2 < sizes[2]);
        case 2:
            assert(s1 < sizes[1]);
        case 1:
            assert(s0 < sizes[0]);
        }
        assert(ind < data.size);
#endif
        return data[ind];
    }
                        operator mdarray<const T, Dim>() const {
        mdarray<const T, Dim> res;
        res.data = data;
        res.indexer = indexer;
        memcpy(res.sizes, sizes, Dim * sizeof(size_t));
        return res;
    }
    mdarray<T, Dim - 1> slice       (size_t dim, int pos) const {
        mdarray<T, Dim - 1> sl;
        sl.data = data;
        for(size_t i = 0; i < Dim - 1; ++i)
            if (i < dim)
                sl.sizes[i] = sizes[i];
            else
                sl.sizes[i] = sizes[i + 1];
        sl.indexer = indexer.slice(dim, pos);
        return sl;
    }
    size_t              volume      () const {
        size_t v = 1;
        for (size_t i = 0; i < Dim; ++i)
            v *= sizes[i];
        return v;
    }
    
private:
    void init(const size_t *s) {
        for (size_t i = 0; i < Dim; ++i) {
            assert(s[i]);
            sizes[i] = s[i];
        }
    }
};

//-------------------------------------------------------------------------------------
template<typename T>
mdarray<T, 2> transpose(const mdarray<T, 2>& arr)
{
    mdarray<T, 2> res = arr;
    std::swap(res.indexer.coeffs[0], res.indexer.coeffs[1]);
    return res;
}

template<typename T, size_t Dim>
mdarray<T, Dim> permute(const mdarray<T, 2>& arr, const size_t* dim)
{
    mdarray<T, Dim> res = arr;
    res.indexer = permute(res.indexer, dim);
    return res;    
}

//-------------------------------------------------------------------------------------
template<typename T>
std::ostream& operator<< (std::ostream& s, const mdarray<T, 1>& a)
{
    int w = s.width();
    for (size_t i = 0; i < a.sizes[0]; ++i)
        s << setw(w) << a(i);
    return s << endl;
}

template<typename T>
std::ostream& operator<< (std::ostream& s, const mdarray<T, 2>& a)
{
    int w = s.width();
    for (size_t i = 0; i < a.sizes[0]; ++i) {
        for (size_t j = 0; j < a.sizes[1]; ++j)
            s << std::setw(w) << a(i,j);
        s << std::endl;
    }
    return s;
}

template<typename T, size_t Dim>
std::ostream& operator<< (std::ostream& s, const mdarray<T, Dim>& a)
{
    int w = s.width();
    for (size_t i = 0; i < a.sizes[Dim - 1]; ++i)
        s << std::setw(w) << a.slice(Dim - 1, i) << std::endl;
    return s;
}

}

#endif