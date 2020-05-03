// Datastructures.cc

#include "datastructures.hh"

#include <random>
#include <cmath>
#include <stdexcept>
#include <stack>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}


Datastructures::Datastructures()
{
    // Replace this comment with your implementation
}

Datastructures::~Datastructures()
{
    // Replace this comment with your implementation
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
    routesByID.clear();
    stopEdges.clear();
    path = {};
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

        std::shared_ptr<routeEdge> newRouteEdge = std::make_shared<routeEdge>();
        routeStop newRouteStop = {id,stopPtr,newRouteEdge};
        stopEdges.insert(std::pair<StopID, routeStop>(id, newRouteStop));
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
    // stops object itself
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
    auto it1 = stopsByID.find(id1);
    auto it2 = stopsByID.find(id2);
    if(it1 == stopsByID.end() || it2 == stopsByID.end()) return NO_REGION;
    return NO_REGION;
}

// ###############################################
// Phase 2 operations
// ###############################################

std::vector<RouteID> Datastructures::all_routes()
{
    std::vector<RouteID> routes = {};
    for(auto& route : routesByID)
    {
        routes.push_back(route.first);
    }
    return routes;
}

bool Datastructures::add_route(RouteID id, std::vector<StopID> stops)
{   
    auto it = routesByID.find(id);
    if(it != routesByID.end()) return false; // regionID on jo olemassa
    if((int)stops.size() < 2) return false; // annettuja pysäkkejä on alle 2
    std::vector<std::pair<StopID,routeStop*>> routeStops;
    auto previousID = stops.begin();
    std::unordered_map<StopID,routeStop>::iterator itPrev = stopEdges.find(*previousID);
    std::unordered_map<StopID,routeStop>::iterator itCur;
    for(auto currentID = stops.begin() + 1; currentID != stops.end(); ++currentID)
    {
        itCur = stopEdges.find(*currentID);
        if(itPrev == stopEdges.end() || itCur == stopEdges.end()) return false;
        Distance dist = getDistance(itPrev->second.stop_->coord_, itCur->second.stop_->coord_);
        itPrev->second.toIDbyRoute_.insert(std::make_pair(id,std::make_pair(*currentID,dist)));
        routeStops.push_back(std::make_pair(*previousID,&itPrev->second));
        previousID = currentID;
        itPrev = itCur;
    }
    // last stop
    itCur->second.toIDbyRoute_.insert(std::make_pair(id,std::make_pair(NO_STOP, NO_DISTANCE)));
    routeStops.push_back(std::make_pair(*previousID,&itCur->second));

    Route newRoute = {id,routeStops};
    routesByID.insert(std::make_pair(id,newRoute));
    return true;
}

std::vector<std::pair<RouteID, StopID>> Datastructures::routes_from(StopID stopid)
{
    auto it = stopEdges.find(stopid);
    if(it == stopEdges.end()) return {{NO_ROUTE,NO_STOP}};
    std::vector<std::pair<RouteID, StopID>> result = {};
    for(auto route : it->second.toIDbyRoute_)
    {
        result.push_back(std::make_pair(route.first, route.second.first));
    }
    if((int)result.size() < 1) return {{NO_ROUTE,NO_STOP}};
    else return result;
}

std::vector<StopID> Datastructures::route_stops(RouteID id)
{
    auto it = routesByID.find(id);
    if(it == routesByID.end()) return {NO_STOP};
    std::vector<StopID> result = {};
    for(auto stop : it->second.stops_)
    {
        result.push_back(stop.first);
    }
    return result;
}

void Datastructures::clear_routes()
{
    routesByID.clear();
    stopEdges.clear();
    path = {};
}

std::vector<std::tuple<StopID, RouteID, Distance>> Datastructures::journey_any(StopID fromstop, StopID tostop)
{
    return journey_least_stops(fromstop, tostop);
}

std::vector<std::tuple<StopID, RouteID, Distance>> Datastructures::journey_least_stops(StopID fromstop, StopID tostop)
{
    if(fromstop == tostop) return {{tostop, NO_ROUTE, 0}}; // sama pysäkki
    auto itFrom = stopEdges.find(fromstop);
    auto itTo = stopEdges.find(tostop);
    if(itFrom == stopEdges.end() || itTo == stopEdges.end()) return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};
    std::vector<std::tuple<StopID, RouteID, Distance>> result = {};
    std::queue<routeStop*> Q;
    routeStop* parent;
    routeStop* child;
    routeEdge* p; // pointteri jolla iteroidaan lapsipysäkkejä
    bool toStopFound = false;
    initStops();
    // start BFS
    itFrom->second.colour_ = Colour::grey;
    Q.push(&itFrom->second);
    while(!Q.empty() && !toStopFound)
    {
        parent = Q.front();
        for(auto& stop : parent->toIDbyRoute_)
        {
            if(stop.second.first == NO_STOP) continue;// parent on reitin viimeinen pysäkki
            child = &stopEdges[stop.second.first]; // voisi antaa suoraan pointterin ID:n tilalla
            if(child->colour_ == Colour::white)
            {
                p = &(*child->routeEdge_);
                child->colour_ = Colour::grey;
                p->fromID_ = &parent->thisID_;
                p->fromEdge_ = &(*parent->routeEdge_);
                p->toID_ = &child->thisID_;
                p->route_ = &stop.first;
                p->dist_ = &stop.second.second;
                if(child->thisID_ == tostop)
                {
                    toStopFound = true;
                    break;
                }
                Q.push(child);
            }
        }
        // merkintä mustaksi
        Q.pop();
    }
    if(toStopFound)
    {
        Distance startDistance = 0;
        getPath(p, result, startDistance);
    }
    return result;
}


std::vector<std::tuple<StopID, RouteID, Distance>> Datastructures::journey_with_cycle(StopID fromstop)
{
    // Replace this comment and the line below with your implementation
    return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};
}

std::vector<std::tuple<StopID, RouteID, Distance>> Datastructures::journey_shortest_distance(StopID fromstop, StopID tostop)
{
    // Replace this comment and the line below with your implementation
    return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};
}

bool Datastructures::add_trip(RouteID routeid, std::vector<Time> const& stop_times)
{
    // Replace this comment and the line below with your implementation
    return false;
}

std::vector<std::pair<Time, Duration>> Datastructures::route_times_from(RouteID routeid, StopID stopid)
{
    // Replace this comment and the line below with your implementation
    return {{NO_TIME, NO_DURATION}};
}

std::vector<std::tuple<StopID, RouteID, Time> > Datastructures::journey_earliest_arrival(StopID fromstop, StopID tostop, Time starttime)
{
    // Replace this comment and the line below with your implementation
    return {{NO_STOP, NO_ROUTE, NO_TIME}};
}

void Datastructures::add_walking_connections()
{
    // Replace this comment and the line below with your implementation
}

// ###############################################
// Phase 1 private operations
// ###############################################
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

// ###############################################
// Phase 1 private operations END
// ###############################################
// Phase 2 private operations
// ###############################################

Distance Datastructures::getDistance(Coord &c1, Coord &c2)
{
    int dy = c2.y - c1.y;
    int dx = c2.x - c1.x;
    double distance = sqrt(dx*dx + dy*dy);
    return (int)floor(distance);
}

void Datastructures::initStops()
{
    routeEdge* p;
    std::unordered_map<StopID,routeStop>::iterator stop;
    for( stop = stopEdges.begin(); stop != stopEdges.end(); ++stop)
    {
        p = &(*stop->second.routeEdge_);
        stop->second.colour_ = Colour::white;
        p->fromEdge_ = nullptr;
        p->fromID_ = nullptr;
        p->toID_ = nullptr;
        p->route_ = nullptr;
        p->dist_ = nullptr;
    }
    path = {};
}

//void Datastructures::goThroughPath(Datastructures::res &result)
//{
//    Distance cumDist = 0;
//    routeEdge* p = path.top();
//    // route start stop
//    result.push_back({*p->fromID_, NO_ROUTE, cumDist});
//    while(!path.empty())
//    {
//        p = path.top();
//        cumDist += *p->dist_;
//        result.push_back({*p->toID_, *p->route_, cumDist});
//        path.pop();
//    }
//}

//void Datastructures::getPath(Datastructures::routeEdge *endStop)
//{
//    if(endStop->fromID_ == nullptr) // start stop
//    {
//        //path.push(&(*endStop));
//        return;
//    }
//    else
//    {
//        path.push(&(*endStop));
//        getPath(endStop->fromEdge_);
//    }
//}

void Datastructures::getPath(Datastructures::routeEdge *endStop, Datastructures::res &result, Distance &cumDist)
{
    routeEdge* parent = endStop->fromEdge_;

    if(parent->fromID_ == nullptr) // start stop
    {
        result.push_back({*endStop->fromID_, NO_ROUTE, cumDist});
        cumDist += *endStop->dist_;
        result.push_back({*endStop->toID_, *endStop->route_, cumDist});
        return;
    }
    else
    {
        getPath(endStop->fromEdge_, result, cumDist);
        cumDist += *endStop->dist_;
        result.push_back({*endStop->toID_, *endStop->route_, cumDist});
    }
}





