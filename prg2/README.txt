
Tiraka prg2
Tekijä: Joonas Puumala

Harjoitustyön ensimmäistä versiota käytettin hyväksi lisäämällä add_stop kutsuun tarvittavat toiminnot toista vaihetta varten. Toisessakin vaiheessa ideana on maksaa tallennustilan käytöllä ohjelman tehokkuuden saamiseksi hyvälle tasolle. Pointtereita käytetään kuitenkin paljon, jotta tallennustilaa tarvitaan mahdollisimman vähän. 

Pysäkkien välisiä reittejä kuvataan graafilla.Ohjelmassa on luotuna unordered_map säiliöt reiteille ja reittipysäkeille. Rakenne on valittu keskimäärin nopean find-toiminnon saamiseksi käyttöön ja lisäksi tarvetta järjestyksen tietämisestä ei ole, koska kyseessä on graafi. Ainoastaan reitin pysäkit on tallennettuna vektoriin, järjestyksen tietämiseksi.

Pysäkeille ja niiden välisille kaarille on luotuna omat rakenteensa, vaikka ne sisältävätkin osin samaa dataa. Kaarien(routeEdge) data muuttuu jokaisella hakukerralla, mutta solmujen (routeStop) data pysyy melko staattisena. Tällöin dataa ei tarvitse liikutella paljon vaan käyttää osoittimia oikeisiin pakíkkoihin. 
HUOM! Koodissa käytetyt parent-child rakenteet kuvaavat mistä-mihin -suhdetta.

Ei-pakollisista metodeista ohjelmaan on toteutettu journey_least_stops, journey_with_cycle ja add_trip

journey_least_stops on toteutettu BFS hakuna, jolloin lyhin reitti löytyy verrattaen nopeasti.
journey_with_cycle on toteutettu DFS hakuna, jolloin mahdollinen sykli on mahdollsita löytää nopeasti.

Tietorakentet:
    struct routeEdge{
        routeEdge* fromEdge_
        StopID* fromID_
        StopID* toID_
        const RouteID* route_
        Distance* dist_ = nullptr;
    }

    // sisältää pysäkiltä lähtevien reittien seuraavat stopID:t
    // pysyvämpi rakenne kuin routeEdge. Ei muutu hakujen välissä.
    struct routeStop{
        StopID thisID_;
        Stop* stop_;
        std::shared_ptr<routeEdge> routeEdge_;
        Status status_ = notVisited;
        std::unordered_map<RouteID,std::pair<StopID,Distance>> toIDbyRoute_; // eri reittien seuraava pysäkki
    };

    // kokonainen reitti
    struct Route{
        RouteID id_;
        std::vector<std::pair<StopID,routeStop*>> stops_; // reitin oikea järjestys
    };

    std::unordered_map<RouteID,Route> routesByID;
    std::unordered_map<StopID,routeStop> stopEdges;
