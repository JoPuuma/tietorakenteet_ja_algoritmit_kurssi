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
    // clear region datastructure
}

std::vector<StopID> Datastructures::all_stops()
{
    std::vector<StopID> stops = {};
    for(auto stop : stopsByID)
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

//    // sort by stop coord
//    std::sort(stops.begin(),stops.end(),
//              [&](const auto& s1, const auto& s2)
//    {
//        Coord coord1 = stopsByID[s1].coord_;
//        Coord coord2 = stopsByID[s2].coord_;
//        if(coord1 != coord2){
//            if(pow(coord1.x,2) + pow(coord1.y,2) < pow(coord2.x,2) + pow(coord2.y,2))
//            {

//            }
//        }
//        else {return false};

//    });

    std::vector<const Stop*> stopPtrs = {};
    for(const auto &stop : stopsByID)
    {
        const Stop* ptr = &stop.second;
        stopPtrs.push_back(ptr);
    }
    // sort by stop coordinate
    std::sort(stopPtrs.begin(),stopPtrs.end(),
              [](const auto& s1, const auto& s2)
    {

    });
    std::vector<StopID> ordered = {};
    for(auto ptr : stopPtrs)
    {
        ordered.push_back(ptr->id_);
    }

    return ordered;
}

StopID Datastructures::min_coord()
{
    // Replace this comment and the line below with your implementation
    return NO_STOP;
}

StopID Datastructures::max_coord()
{
    // Replace this comment and the line below with your implementation
    return NO_STOP;
}

std::vector<StopID> Datastructures::find_stops(Name const& name)
{
    // Replace this comment and the line below with your implementation
    return {NO_STOP};
}

bool Datastructures::change_stop_name(StopID id, const Name& newname)
{
    // Replace this comment and the line below with your implementation
    return false;
}

bool Datastructures::change_stop_coord(StopID id, Coord newcoord)
{
    // Replace this comment and the line below with your implementation
    return false;
}

bool Datastructures::add_region(RegionID id, const Name &name)
{
    // Replace this comment and the line below with your implementation
    return false;
}

Name Datastructures::get_region_name(RegionID id)
{
    // Replace this comment and the line below with your implementation
    return NO_NAME;
}

std::vector<RegionID> Datastructures::all_regions()
{
    // Replace this comment and the line below with your implementation
    return {NO_REGION};
}

bool Datastructures::add_stop_to_region(StopID id, RegionID parentid)
{
    // Replace this comment and the line below with your implementation
    return false;
}

bool Datastructures::add_subregion_to_region(RegionID id, RegionID parentid)
{
    // Replace this comment and the line below with your implementation
    return false;
}

std::vector<RegionID> Datastructures::stop_regions(StopID id)
{
    // Replace this comment and the line below with your implementation
    return {NO_REGION};
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
