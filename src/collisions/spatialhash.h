#pragma once
#include <vector>
#include <cmath>
#include <cstdint>

namespace morph{ namespace softmats{
/**
 * Spatial hash of point indices, used to find the points near a face.
 *
 * Different cells can share a bucket, so a query can return points that are
 * not in the cell; the caller is expected to check the candidates.
 *
 * @author Alejandro Jimenez Rodriguez
 * @see Teschner, M., Heidelberger, B., Müller, M., Pomerantes, D., & Gross, M. H. (2003). Optimized spatial hashing for collision detection of deformable objects.
 */
class SpatialHash{
private:
    double cellSize;
    int stamp;
    std::vector<std::vector<int>> buckets;
    std::vector<int> stamps;

    size_t bucketOf( int i, int j, int k ) const {
        const uint64_t h = ( static_cast<uint64_t>(static_cast<int64_t>(i))*73856093ULL ) ^
                           ( static_cast<uint64_t>(static_cast<int64_t>(j))*19349663ULL ) ^
                           ( static_cast<uint64_t>(static_cast<int64_t>(k))*83492791ULL );
        return h % buckets.size();
    }
public:
    SpatialHash():cellSize(1.0), stamp(0), buckets(1), stamps(1, -1){}

    // Empties the table and sets its dimensions
    void reset( double cellSize, size_t numBuckets ){
        this->cellSize = cellSize;

        if( buckets.size() != numBuckets ){
            buckets.assign( numBuckets, std::vector<int>() );
            stamps.assign( numBuckets, -1 );
        }

        // Buckets are emptied lazily, when first used after a reset
        ++stamp;
    }

    int cell( double a ) const {
        return static_cast<int>( std::floor( a/cellSize ) );
    }

    void insert( double x, double y, double z, int index ){
        const size_t b = bucketOf( cell(x), cell(y), cell(z) );

        if( stamps[b] != stamp ){
            buckets[b].clear();
            stamps[b] = stamp;
        }

        buckets[b].push_back( index );
    }

    // Points stored in the bucket of the given cell. nullptr when it is empty
    const std::vector<int>* query( int i, int j, int k ) const {
        const size_t b = bucketOf( i, j, k );
        return ( stamps[b] == stamp && !buckets[b].empty() ) ? &buckets[b] : nullptr;
    }
};

}}
