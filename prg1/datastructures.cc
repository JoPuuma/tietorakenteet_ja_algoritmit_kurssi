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
    stopCoords.clear();
    stopsByName.clear();
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
        std::shared_ptr<Stop> newStop = std::make_shared<Stop>(Stop{id, xy, name});
        stopsByID[id] = newStop;

        Stop* stopPtr = &(*newStop);
        stopCoords.insert(std::pair<Coord, Stop*>(xy,stopPtr));
        stopsByName.insert(std::pair<std::string,Stop*>(name,stopPtr));
        return true;
    }
}

Name Datastructures::get_stop_name(StopID id)
{
    auto it = stopsByID.find(id);
    if(it == stopsByID.end()) return NO_NAME;
    else return it->second->name_;
}

Coord Datastructures::get_stop_coord(StopID id)
{
    auto it = stopsByID.find(id);
    if(it == stopsByID.end()) return NO_COORD;
    else return it->second->coord_;
}

std::vector<StopID> Datastructures::stops_alphabetically()
{
    std::vector<StopID> ordered = {};
    for(auto stop : stopsByName)
    {
        ordered.push_back(stop.second->id_);
    }

    return ordered;
}


std::vector<StopID> Datastructures::stops_coord_order()
{
    std::vector<StopID> ordered = {};
    for(auto stop : stopCoords)
    {
        ordered.push_back(stop.second->id_);
    }

    return ordered;
}

StopID Datastructures::min_coord()
{
    if(stop_count() == 0) return NO_STOP;
    return stopCoords.begin()->second->id_;
}

StopID Datastructures::max_coord()
{
    if(stop_count() == 0) return NO_STOP;
    return stopCoords.rbegin()->second->id_;
}

std::vector<StopID> Datastructures::find_stops(Name const& name)
{
    std::vector<StopID> stops = {};
    std::pair <std::multimap<std::string,Stop*>::iterator, std::multimap<std::string,Stop*>::iterator> ret;
    ret = stopsByName.equal_range(name);
    for (std::multimap<std::string,Stop*>::iterator it=ret.first; it!=ret.second; ++it)
    {
      stops.push_back(it->second->id_);
    }
    return stops;
}

bool Datastructures::change_stop_name(StopID id, const Name& newname)
{
    auto stopIt = stopsByID.find(id);

    if(stopIt == stopsByID.end()) return false;
//    else
//    {
//        stopIt->second->name_ = newname;
//    }
    // stopsByName
    Stop* stopPtr = &(*stopIt->second);
    std::pair <std::multimap<std::string,Stop*>::iterator, std::multimap<std::string,Stop*>::iterator> nameRet;
    nameRet = stopsByName.equal_range(stopIt->second->name_);
    for (std::multimap<std::string,Stop*>::iterator it=nameRet.first; it!=nameRet.second; ++it)
    {
        if(it->second->id_ == id)
        {
            stopsByName.erase(it);
            stopsByName.insert(std::make_pair(newname, stopPtr));
            break;
        }
    }
    stopIt->second->name_ = newname;
    return true;
}

bool Datastructures::change_stop_coord(StopID id, Coord newcoord)
{
    auto stopIt = stopsByID.find(id);

    if(stopIt == stopsByID.end()) return false;
    else
    {
        std::pair <std::multimap<Coord,Stop*>::iterator, std::multimap<Coord,Stop*>::iterator> ret;
        Stop* stopPtr = &(*stopIt->second);
        // update new coord to the stopCoords structure
        ret = stopCoords.equal_range(stopPtr->coord_);
        for (std::multimap<Coord,Stop*>::iterator it=ret.first; it!=ret.second; ++it)
        {
            if(it->second->id_ == id)
            {
                stopCoords.erase(it);
                stopCoords.insert(std::make_pair(newcoord,stopPtr));
                break;
            }
        }
        // update new coord to the regoin of the stop
        if(stopIt->second->region_ != nullptr)
        {
            std::multimap<Coord, Stop*>* m = &(stopIt->second->region_->stops_);
            ret = m->equal_range(stopIt->second->coord_);
            for (std::multimap<Coord,Stop*>::iterator it=ret.first; it!=ret.second; ++it)
            {
                if(it->second->id_ == id)
                {
                    Region* regionPtr = &(*stopPtr->region_);
                    regionPtr->stops_.erase(it);
                    regionPtr->stops_.insert(std::make_pair(newcoord,stopPtr));
                    regionPtr->minMaxIsValid = false;
                    break;
                }
            }
        }
        // update stopsByID (stop itself)
        stopPtr->coord_ = newcoord;

        return true;
    }
}

bool Datastructures::add_region(RegionID id, const Name &name)
{
    if(regionsByID.find(id) == regionsByID.end())
    {
        Region newRegion = {id, name, nullptr, {},{},{},false};
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
    Stop* stopPtr = &(*stopIt->second);
    if(stopPtr->region_ != nullptr) return false;
    // region to stop
    stopPtr->region_ = &regionIt->second;
    // stop to region
    regionIt->second.stops_.insert(std::make_pair(stopPtr->coord_,stopPtr));
    regionIt->second.minMaxIsValid = false;
    return true;
}

bool Datastructures::add_subregion_to_region(RegionID id, RegionID parentid)
{
    auto subIt = regionsByID.find(id);
    auto parentIt = regionsByID.find(parentid);

    if(subIt == regionsByID.end() || parentIt == regionsByID.end()) return false;

    Region* subRegion = &subIt->second;
    if(subRegion->overRegion_ != nullptr) return false; // has already overregion

    Region* parentRegion = &parentIt->second;
    Region* parentOverRegion = parentRegion->overRegion_;
    if(parentOverRegion != nullptr)
    {
        if( parentOverRegion->id_ == subRegion->id_) return false; // cannot make loop between regions
    }

    // add regions to datastructure elements
    subRegion->overRegion_ = parentRegion;
    parentRegion->subRegions_.push_back(subRegion);
    parentRegion->minMaxIsValid = false;
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
    getRegions(regions, stop->second->region_);
    return regions;
}

void Datastructures::creation_finished()
{
    // Replace this comment with your implementation
    // You don't have to use this method for anything, if you don't need it
}

std::pair<Coord,Coord> Datastructures::region_bounding_box(RegionID id)
{
    std::pair<Coord,Coord> minMax = {NO_COORD, NO_COORD};
    auto regionIt = regionsByID.find(id);
    if(regionIt == regionsByID.end()) return minMax;
    Region* regionPtr = &(regionIt->second);
    getMinMaxCoords(minMax.first, minMax.second, regionPtr);
    return minMax;
}

std::vector<StopID> Datastructures::stops_closest_to(StopID id)
{
    (void)id;
    // Replace this comment and the line below with your implementation
    return {NO_STOP};
}

bool Datastructures::remove_stop(StopID id)
{
    auto stopIt = stopsByID.find(id);
    if(stopIt == stopsByID.end()) return false;
    Stop* stopPtr = &(*stopIt->second);
    Coord* coord = &stopPtr->coord_;
    std::pair <std::multimap<Coord,Stop*>::iterator, std::multimap<Coord,Stop*>::iterator> ret;
    std::pair <std::multimap<std::string,Stop*>::iterator, std::multimap<std::string,Stop*>::iterator> nameRet;

    // stopsByName
    nameRet = stopsByName.equal_range(stopIt->second->name_);
    for (std::multimap<std::string,Stop*>::iterator it=nameRet.first; it!=nameRet.second; ++it)
    {
        if(it->second->id_ == id)
        {
            stopsByName.erase(it);
            break;
        }
    }
    // stopCoords
    ret = stopCoords.equal_range(*coord);
    for (std::multimap<Coord,Stop*>::iterator it=ret.first; it!=ret.second; ++it)
    {
        if(it->second->id_ == id)
        {
            stopCoords.erase(it);
            break;
        }
    }
    // region of the stop
    if(stopPtr->region_ != nullptr)
    {
        std::multimap<Coord, Stop*>* m = &(stopPtr->region_->stops_);
        ret = m->equal_range(stopPtr->coord_);
        for (std::multimap<Coord,Stop*>::iterator it=ret.first; it!=ret.second; ++it)
        {
            if(it->second->id_ == id)
            {
                Region* regionPtr = &(*stopPtr->region_);
                regionPtr->stops_.erase(it);
                break;
            }
        }
        updateRegionMinMax(stopPtr->region_ );
    }
    // stopsByID
    stopsByID.erase(stopIt);
    return true;
}

RegionID Datastructures::stops_common_region(StopID id1, StopID id2)
{
    (void)id1;
    (void)id2;
    // Replace this comment and the line below with your implementation
    return NO_REGION;
}

bool Datastructures::isSmaller(Coord c1, Coord c2)
{
        if (c1.y < c2.y) { return true; }
        else if (c2.y < c1.y) { return false; }
        else { return c1.x < c2.x; }
}

void Datastructures::getRegions(std::vector<RegionID> &regions, Datastructures::Region* overRegionPtr)
{
    if(overRegionPtr == nullptr) return;

    RegionID id = overRegionPtr->id_;
    regions.push_back(id);
    if(overRegionPtr->overRegion_ == nullptr)
    {
        return;
    }
    else
    {
        getRegions(regions, overRegionPtr->overRegion_);
    }
}

void Datastructures::getMinMaxCoords(Coord &min, Coord &max, Datastructures::Region *regionPtr)
{
    // initialize if min or max has not valid value and they will be compared
    if( (min == NO_COORD || max == NO_COORD) && regionPtr->stops_.size() > 0  )
    {
        min = regionPtr->stops_.begin()->first;
        max = regionPtr->stops_.rbegin()->first;
    }
    // compare with region own stops
    if( regionPtr->stops_.size() > 0 )
    {
        if(!(regionPtr->minMaxIsValid))
        {
            updateRegionMinMax(regionPtr); // theta(n)
        }
        Coord* regionMin = &(regionPtr->minMax.first);
        Coord* regionMax = &(regionPtr->minMax.second);
        if(min.x > regionMin->x) min.x = regionMin->x;
        if(min.y > regionMin->y) min.y = regionMin->y;
        if(max.x < regionMax->x) max.x = regionMax->x;
        if(max.y < regionMax->y) max.y = regionMax->y;
    }
    // call recursively if subregions exist
    if((int)regionPtr->subRegions_.size() > 0)
    {
        for(auto subRegion : regionPtr->subRegions_)
        {
            getMinMaxCoords(min, max, subRegion);
        }
    }
    return;
}

void Datastructures::updateRegionMinMax(Datastructures::Region *regionPtr)
{
    if(regionPtr->stops_.size() == 0)
    {
        regionPtr->minMaxIsValid = true;
        return;
    }
    Coord rmin = regionPtr->stops_.begin()->first;
    Coord rmax = regionPtr->stops_.rbegin()->first;
    for(auto& stop : regionPtr->stops_)
    {
        const Coord* coord = &(stop.first);
        if(rmin.x > coord->x) rmin.x = coord->x;
        if(rmin.y > coord->y) rmin.y = coord->y;
        if(rmax.x < coord->x) rmax.x = coord->x;
        if(rmax.y < coord->y) rmax.y = coord->y;
    }
    regionPtr->minMax = std::make_pair(rmin,rmax);
    regionPtr->minMaxIsValid = true;
}



