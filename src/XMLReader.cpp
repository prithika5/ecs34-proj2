#include "XMLReader.h"


struct CXMLReader::SImplementation{
    bool DEnd;

    SImplementation(std::shared_ptr< CDataSource > src){
        DEnd = false;
        (void)src; // unused for now
    }

    ~SImplementation(){

    }

    bool End() const{
        return DEnd;
    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata){
        (void)entity;
        (void)skipcdata;
        return false; 
    }
};

CXMLReader::CXMLReader(std::shared_ptr< CDataSource > src){
    DImplementation = std::make_unique<SImplementation>(src);
}

CXMLReader::~CXMLReader(){

}

bool CXMLReader::End() const{
    return DImplementation->End();
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata){
    return DImplementation->ReadEntity(entity, skipcdata);
}
