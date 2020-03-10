// Datastructures.cc

#include "datastructures.hh"

#include <random>
#include <cmath>
#include <stdexcept>
#include <unordered_map>
#include <algorithm>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures():
    stopsByID({})
{

}

Datastructures::~Datastructures()
{

}

int Datastructures::stop_count()
{
    return stopsByID.size();
}

void Datastructures::clear_all()
{
    stopsByID.clear();
    regionsByID.clear();
}

std::vector<StopID> Datastructures::all_stops()
{
    std::vector<StopID> stops = {};
    for(auto& stop : stopsByID)
    {
        stops.push_back(stop.first);
    }
    return stops;
}

bool Datastructures::add_stop(StopID id, const Name& name, Coord xy)
{
    if(stopsByID.find(id) != stopsByID.end())
    {
        return false;
    }
    else
    {
        Stop newStop = {id, xy, name};
        stopsByID[id] = newStop;
        return true;
    }
}

Name Datastructures::get_stop_name(StopID id)
{
    auto it = stopsByID.find(id);
    if(it == stopsByID.end()) return NO_NAME;
    else return it->second.name_;
}

Coord Datastructures::get_stop_coord(StopID id)
{
    auto it = stopsByID.find(id);
    if(it == stopsByID.end()) return NO_COORD;
    else return it->second.coord_;
}

std::vector<StopID> Datastructures::stops_alphabetically() // 200 10 000 ~ 2.5s
{
    std::vector<const Stop*> stopPtrs = {};
    for(const auto &stop : stopsByID)
    {
        const Stop* ptr = &stop.second;
        stopPtrs.push_back(ptr);
    }
    // sort by stop name
    std::sort(stopPtrs.begin(),stopPtrs.end(),
              [](const auto& s1, const auto& s2)
    {
        return s1->name_ < s2->name_;
    });
    std::vector<StopID> ordered = {};
    for(auto ptr : stopPtrs)
    {
        ordered.push_back(ptr->id_);
    }

    return ordered;
}


std::vector<StopID> Datastructures::stops_coord_order()
{

    std::vector<const Stop*> stopPtrs = {};
    for(const auto &stop : stopsByID)
    {
        const Stop* ptr = &stop.second;
        stopPtrs.push_back(ptr);
    }
    // sort by stop coordinate
    std::sort(stopPtrs.begin(),stopPtrs.end(),
              [this](const auto& s1, const auto& s2)
    {
        return isSmaller(s1->coord_,s2->coord_);
    });
    std::vector<StopID> ordered = {};
    for(auto ptr : stopPtrs)
    {
        ordered.push_back(ptr->id_);
    }

    return ordered;
}

StopID Datastructures::min_coord() // 200 100000 2.5s
{
    if(stop_count() == 0) return NO_STOP;
    auto it = std::min_element(stopsByID.begin(),stopsByID.end(),[this](const auto& s1, const auto& s2)
    {
        return isSmaller(s1.second.coord_,s2.second.coord_);
    });
    return it->first;
}

StopID Datastructures::max_coord()
{
    if(stop_count() == 0) return NO_STOP;
    auto it = std::max_element(stopsByID.begin(),stopsByID.end(),[this](const auto& s1, const auto& s2)
    {
        return isSmaller(s1.second.coord_,s2.second.coord_);
    });
    return it->first;
}

std::vector<StopID> Datastructures::find_stops(Name const& name)
{
    std::vector<StopID> stops = {};
    for(const auto& stop : stopsByID)
    {
        if(stop.second.name_ == name)
        {
            stops.push_back(stop.first);
        }
    }
    return stops;
}

bool Datastructures::change_stop_name(StopID id, const Name& newname)
{
    auto stopIt = stopsByID.find(id);

    if(stopIt == stopsByID.end()) return false;
    else
    {
        stopIt->second.name_ = newname;
        return true;
    }
}

bool Datastructures::change_stop_coord(StopID id, Coord newcoord)
{
    auto stopIt = stopsByID.find(id);

    if(stopIt == stopsByID.end()) return false;
    else
    {
        stopIt->second.coord_ = newcoord;
        return true;
    }
}

bool Datastructures::add_region(RegionID id, const Name &name)
{
    if(regionsByID.find(id) == regionsByID.end())
    {
        Region newRegion = {id, name, nullptr, {}};
        regionsByID[id] = newRegion;
        return true;
    }
    else return false;
}

Name Datastructures::get_region_name(RegionID id)
{
    auto it = regionsByID.find(id);
    if(it == regionsByID.end()) return NO_NAME;
    else return it->second.name_;
}

std::vector<RegionID> Datastructures::all_regions()
{
    std::vector<RegionID> regions = {};
    for(auto& region : regionsByID)
    {
        regions.push_back(region.first);
    }
    return regions;
}

bool Datastructures::add_stop_to_region(StopID id, RegionID parentid)
{
    auto stopIt = stopsByID.find(id);
    auto regionIt = regionsByID.find(parentid);

    if(stopIt == stopsByID.end() || regionIt == regionsByID.end()) return false;
    if(stopIt->second.region_ != nullptr) return false;

    stopIt->second.region_ = &regionIt->second;
    return true;
}

bool Datastructures::add_subregion_to_region(RegionID id, RegionID parentid)
{
    auto subIt = regionsByID.find(id);
    auto parentIt = regionsByID.find(parentid);

    if(subIt == regionsByID.end() || parentIt == regionsByID.end()) return false;

    Region* subRegion = &subIt->second;
    if(subRegion->overRegion_ != nullptr) return false; // have already overregion

    Region* parentRegion = &parentIt->second;

    // add regions to datastructure elements
    subRegion->overRegion_ = parentRegion;
    parentRegion->subRegions_.push_back(subRegion);
    return true;
}

std::vector<RegionID> Datastructures::stop_regions(StopID id)
{
    std::vector<RegionID> regions = {};
    auto stop = stopsByID.find(id);

    if(stop == stopsByID.end())
    {
        regions.push_back(NO_REGION);
        return regions;
    }
    getRegions(regions, stop->second.region_);
    return regions;
}

void Datastructures::creation_finished()
{
    // Replace this comment with your implementation
    // You don't have to use this method for anything, if you don't need it
}

std::pair<Coord,Coord> Datastructures::region_bounding_box(RegionID id)
{
    // Replace this comment and the line below with your implementation
    return {NO_COORD, NO_COORD};
}

std::vector<StopID> Datastructures::stops_closest_to(StopID id)
{
    // Replace this comment and the line below with your implementation
    return {NO_STOP};
}

bool Datastructures::remove_stop(StopID id)
{
    // Replace this comment and the line below with your implementation
    return false;
}

RegionID Datastructures::stops_common_region(StopID id1, StopID id2)
{
    // Replace this comment and the line below with your implementation
    return NO_REGION;
}

bool Datastructures::isSmaller(Coord c1, Coord c2) // return c1 < c2
{
    double d1 = c1.x * c1.x + c1.y * c1.y;
    double d2 = c2.x * c2.x + c2.y * c2.y;

    if(d1 < d2) return true;
    else if(d1 > d2) return false;
    else
    {
        return c1.y < c2.y;
    }
}

void Datastructures::getRegions(std::vector<RegionID> &regions, Datastructures::Region* overRegionPtr)
{
    if(overRegionPtr == nullptr) return;

    RegionID id = overRegionPtr->id_;
    if(overRegionPtr->overRegion_ == nullptr)
    {
        regions.push_back(id);
        return;
    }
    else
    {
        regions.push_back(id);
        getRegions(regions, overRegionPtr->overRegion_);
    }
}


