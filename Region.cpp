//
// Created by Stephen Clyde on 3/4/17.
//

#include "Region.h"
#include "Utils.h"
#include "World.h"
#include "Nation.h"
#include "State.h"
#include "County.h"
#include "City.h"

#include <iostream>
#include <iomanip>

const std::string regionDelimiter = "^^^";
const int TAB_SIZE = 4;
unsigned int Region::m_nextId = 0;

Region* Region::create(std::istream &in)
{
    Region* region = nullptr;
    std::string line;
    std::getline(in, line);
    if (line!="")
    {
        region = create(line);
        if (region!= nullptr)
            region->loadChildren(in);
    }
    return region;
}
Region* Region::create(const std::string& data)
{
    Region* region = nullptr;
    std::string regionData;
    unsigned long commaPos = (int) data.find(",");
    if (commaPos != std::string::npos)
    {
        std::string regionTypeStr = data.substr(0,commaPos);
        regionData = data.substr(commaPos+1);

        bool isValid;
        RegionType regionType = (RegionType) convertStringToInt(regionTypeStr, &isValid);

        if (isValid)
        {
            region = create(regionType, regionData);
        }

    }

    return region;
}

Region* Region::create(RegionType regionType, const std::string& data)
{
    Region* region = nullptr;
    std::string fields[3];
    if (split(data, ',', fields, 3)) {

        // Create the region based on type
        switch (regionType) {
            case WorldType:
                region = new World();
                break;
            case NationType:
                region = new Nation(fields);
                break;
            case StateType:
                region=new State(fields);
                break;
            case CountyType:
                region=new County(fields);
                break;
            case CityType:
                region=new City(fields);
                break;
       // DONE: Add cases for State, County, and City
            default:
                break;
        }//StateType, CountyType, CityType

        // If the region isn't valid, git ride of it
        if (region != nullptr && !region->getIsValid()) {
            delete region;
            region = nullptr;
        }
    }

    return region;
}

std::string Region::regionLabel(RegionType regionType)
{
    std::string label = "Unknown";
    switch (regionType)
    {
        case Region::WorldType:
            label = "World";
            break;
        case Region::NationType:
            label = "Nation";
            break;
        case Region::StateType:
            label = "State";
            break;
        case Region::CountyType:
            label = "County";
            break;
        case Region::CityType:
            label = "City";
            break;
        default:
            break;
    }
    return label;
}

Region::Region() { }

Region::Region(RegionType type, const std::string data[]) :
        m_id(getNextId()), m_regionType(type), m_isValid(true)
{
    m_name = data[0];
    m_population = convertStringToUnsignedInt(data[1], &m_isValid);
    if (m_isValid)
        m_area = convertStringToDouble(data[2], &m_isValid);
}

Region::~Region()
{
    delete[] m_subregion;
    // DONE: cleanup any dynamically allocated objects
}

std::string Region::getRegionLabel() const
{
    return regionLabel(getType());
}

unsigned int Region::computeTotalPopulation()
{
    long int sum=m_population;
    for(int i=0;i<subCount;i++){
        sum+=m_subregion[i]->computeTotalPopulation();//recursive!
    }
    return sum;
    // DONE: implement computeTotalPopulation, such that the result is m_population + the total population for all sub-regions
}

void Region::list(std::ostream& out)
{
    out << std::endl;
    out <<getId()<<" "<< getName() << ":" << std::endl;

    // DONE: implement the loop in the list method
    for(int i=0;i<subCount;i++){
        m_subregion[i]->list(out);

    }
    //out<<regionDelimiter<<std::endl;
    // foreach subregion, print out
    //      id    name
}

void Region::display(std::ostream& out, unsigned int displayLevel, bool showChild)
{
    if (displayLevel>0)
    {
        out << std::setw(displayLevel * TAB_SIZE) << " ";
    }

    unsigned totalPopulation = computeTotalPopulation();
    double area = getArea();
    double density = (double) totalPopulation / area;

    // DONE: compute the totalPopulation using a method

    out << std::setw(6) << getId() << "  "
        << getName() << ", population="
        << totalPopulation
        << ", area=" << area
        << ", density=" << density << std::endl;

    if (showChild)
    {
        // DONE: implement loop in display method
        for(int i=0;i<subCount;i++){
            m_subregion[i]->display(out,displayLevel+1,showChild);
        }
        // foreach subregion
        //      display that subregion at displayLevel+1 with the same showChild value
    }
}

void Region::save(std::ostream& out)
{
    out << getType()
        << "," << getName()
        << "," << getPopulation()
        << "," << getArea()
        << std::endl;

    // DONE: implement loop in save method to save each sub-region
    for(int i=0;i<subCount;i++){
        m_subregion[i]->save(out);
    }
    // foreach subregion,
    //      save that region

    out << regionDelimiter << std::endl;
}

void Region::validate()
{
    m_isValid = (m_area!=UnknownRegionType && m_name!="" && m_area>=0);
}

void Region::loadChildren(std::istream& in)
{
    std::string line;
    bool done = false;
    while (!in.eof() && !done)
    {
        std::getline(in, line);
        if (line==regionDelimiter)
        {
            done = true;
        }
        else
        {
            Region* child = create(line);
            if (child!= nullptr)
            {
                this->addSubregion(child);
                // TODO: Add the new sub-region to this region
                child->loadChildren(in);
            }
        }
    }
}

unsigned int Region::getNextId()
{
    if (m_nextId==UINT32_MAX)
        m_nextId=1;

    return m_nextId++;
}
void Region::addSubregion(Region* region){
    subCount++;
    Region* holder[subCount];
    for(int i=0;i<subCount-1;i++){
        holder[i]=m_subregion[i];
    }
    holder[subCount-1]=region;
    for(int i=0;i<subCount;i++){
        m_subregion[i]=holder[i];
    }


}
int Region::getSubRegionCount(){
    return subCount;
}

Region* Region::getSubRegionByIndex(int in){
    for(int i=0;i<subCount;i++){
        if (in == m_subregion[i]->getId())return m_subregion[i];
    }
    std::cout<<"subRegion not found"<<std::endl;
}