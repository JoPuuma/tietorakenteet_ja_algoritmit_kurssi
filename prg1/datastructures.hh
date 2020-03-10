// Datastructures.hh

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <utility>
#include <limits>

#include <unordered_map>
#include <set>
#include <memory> // shared_ptr

// Types for IDs
using StopID = long int;
using RegionID = std::string;
using Name = std::string;

// Return values for cases where required thing was not found
StopID const NO_STOP = -1;
RegionID const NO_REGION = "!!NO_REGION!!";

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Return value for cases where name values were not found
Name const NO_NAME = "!!NO_NAME!!";

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};

// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

// Example: Defining < for Coord so that it can be used
// as key for std::map/set
inline bool operator<(Coord c1, Coord c2)
{
    if (c1.y < c2.y) { return true; }
    else if (c2.y < c1.y) { return false; }
    else { return c1.x < c2.x; }
}

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};


// This is the class you are supposed to implement

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: theta(1)
    // Short rationale for estimate: size-funktion toiminta on vakioaikainen
    int stop_count();

    // Estimate of performance: theta(n)
    // Short rationale for estimate: Jokaisen alkion purkajaa kutsutaan
    void clear_all();

    // Estimate of performance: theta(n)
    // Short rationale for estimate: Käydään läpi koko tietorakenne ja kopioidaan alkiot
    std::vector<StopID> all_stops();

    // Estimate of performance: O(n)
    // Short rationale for estimate: Pysäkin luonti ja lisäys tietorakenteeseen vakioaikaisia. Find on lineaarinen.
    bool add_stop(StopID id, Name const& name, Coord xy);

    // Estimate of performance: O(n), keskimäärin theta(1)
    // Short rationale for estimate: find keskimäärin vakioaikainen, mutta pahimmassa tapauksessa lineaarinen
    Name get_stop_name(StopID id);

    // Estimate of performance: O(n), keskimäärin theta(1)
    // Short rationale for estimate: find keskimäärin vakioaikainen, mutta pahimmassa tapauksessa lineaarinen
    Coord get_stop_coord(StopID id);

    // Estimate of performance: O(nlog(n))
    // Short rationale for estimate: Sort funktio on hitain (nlog(n)), mutta aikaa kuluu myös kopiointiin (n).
    std::vector<StopID> stops_alphabetically();

    // Estimate of performance: O(nlog(n))
    // Short rationale for estimate: Sort funktio on hitain (nlog(n)), mutta aikaa kuluu myös kopiointiin (n).
    std::vector<StopID> stops_coord_order();

    // Estimate of performance: theta(n)
    // Short rationale for estimate: min_element funktion ajankäyttö on lineaarinen.
    //                               Vertailuoperaatio lisää absoluutista aikaa.
    StopID min_coord();

    // Estimate of performance: theta(n)
    // Short rationale for estimate: max_element funktion ajankäyttö on lineaarinen.
    //                               Vertailuoperaatio lisää absoluutista aikaa.
    StopID max_coord();

    // Estimate of performance: theta(n)
    // Short rationale for estimate: Kaikki alkiot käydään läpi, joten aikaa kuluun n:n verran
    std::vector<StopID> find_stops(Name const& name);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Aikaa kuluu etsimiseen find-funktioilla maksimissaan n:n verran.
    //                               Muut toiminnot eivät kuluta lähes yhtään aikaa.
    bool change_stop_name(StopID id, Name const& newname);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Aikaa kuluu etsimiseen find-funktioilla maksimissaan n:n verran.
    //                               Muut toiminnot eivät kuluta lähes yhtään aikaa.
    bool change_stop_coord(StopID id, Coord newcoord);

    // Estimate of performance: theta
    // Short rationale for estimate:
    bool add_region(RegionID id, Name const& name);

    // Estimate of performance: O(n), keskimäärin theta(1)
    // Short rationale for estimate: Find-funktio on huonoimmassa tapauksessa lineaarinen, mutta keskimäärin vakioaikainen.
    Name get_region_name(RegionID id);

    // Estimate of performance: theta(n)
    // Short rationale for estimate: kaikki alkiot käydään läpi ja kopioidaan vectoriin.
    std::vector<RegionID> all_regions();

    // Estimate of performance:
    // Short rationale for estimate:
    bool add_stop_to_region(StopID id, RegionID parentid);

    // Estimate of performance:
    // Short rationale for estimate:
    bool add_subregion_to_region(RegionID id, RegionID parentid);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<RegionID> stop_regions(StopID id);

    // Non-compulsory operations

    // Estimate of performance:
    // Short rationale for estimate:
    void creation_finished();

    // Estimate of performance:
    // Short rationale for estimate:
    std::pair<Coord, Coord> region_bounding_box(RegionID id);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<StopID> stops_closest_to(StopID id);

    // Estimate of performance:
    // Short rationale for estimate:
    bool remove_stop(StopID id);

    // Estimate of performance:
    // Short rationale for estimate:
    RegionID stops_common_region(StopID id1, StopID id2);

private:

    struct Region{
        RegionID id_;
        Name name_;
        Region* overRegion_ = nullptr;
        std::vector<Region*> subRegions_;
    };

    struct Stop {
        StopID id_;
        Coord coord_;
        Name name_;
        Region* region_ = nullptr;
    };


    std::unordered_map<StopID,Stop> stopsByID;
    std::unordered_map<RegionID,Region> regionsByID;

    bool isSmaller(Coord c1,Coord c2); // return c1 < c2 (distance from origin)
    void getRegions(std::vector<RegionID>& regions, Region* overRegionPtr);
};

#endif // DATASTRUCTURES_HH
