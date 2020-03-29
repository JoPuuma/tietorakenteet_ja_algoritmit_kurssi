
prg1
toteutus: Joonas Puumala, 263420

Ohjelma on toteutettu mahdollisimman tehokkaaksi nopeuden kannalta, mikä vaatii enemmän muistia käyttöön. Muistinkäyttöä kuitenkin on rajoitettu käyttämällä paljon osoittimia tietorakenteissa.
Ohjelma sisältää useampia tietorakenteita, jotka voidaan pitää järjestyksessä eri tietojen mukaan. Tällöin voidaan nopeuttaa monen erilaisen haun kuluttamaa aikaa.

tietorakenteet:

    std::unordered_map<StopID,std::shared_ptr<Stop>> stopsByID;
    std::unordered_map<RegionID,Region> regionsByID;
    std::multimap<Coord, Stop*> stopCoords;
    std::multimap<std::string,Stop*> stopsByName;

    struct Region{
        RegionID id_;
        Name name_;
        Region* overRegion_ = nullptr;
        std::vector<Region*> subRegions_;
        std::multimap<Coord,Stop*> stops_;
        std::pair<Coord,Coord> minMax;
        bool minMaxIsValid;
    };

    struct Stop {
        StopID id_;
        Coord coord_;
        Name name_;
        Region* region_ = nullptr;
    };

StopsByID ja regionsByID on toteutettu järjestämättöminä, koska id:itä käytetään vain tietyn pysäkin/alueen etsintään. stopCoords ja stopsByName ovat järjestettyjä ja mahdollistavat useamman saman avaimen. 

Erityisesti koordinaatteja tallennetaan useampaan kertaan eri paikkoihin luomalla uusi koordinaatti. Tämä tehdään, jotta säiliöt osaavat järjestää alkiot halutulla tavalla. Tästä johtuen pysäkin tietojen muuttaminen käynnistää melko pitkän tapahtumaketjun, jossa tiedot päivitetään kaikkiin tietorakenteisin. On siis oletettu, ettei esimerkiksi pysäkin nimeä tai koordinaatteja vaihdeta usein. Sama pätee myös pysäkin poistamiseen. Eri operaatioiden asymptoottiset tehokkuudet ja perustelut ovat kirjattuna datasructures.hh tiedostoon.

Ei-pakollisista metodeista ohjelmaan on toeutettu region_bounding_box ja remove_stop.


