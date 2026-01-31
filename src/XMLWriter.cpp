#include "XMLWriter.h"
#include <memory>
#include <vector>
#include <string>

struct CXMLWriter::SImplementation {
    
    std::shared_ptr<CDataSink> DSink;
    std::vector<std::string> Open;

    SImplementation(std::shared_ptr<CDataSink> sink) {
        DSink = sink;
    }

    ~SImplementation() {
    }

    bool WriteEntity(const SXMLEntity &) {
        return true;
    }

    bool Flush() {
        return true;
    }
};

CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink) {
    DImplementation = std::make_unique<SImplementation>(sink);
}

CXMLWriter::~CXMLWriter() {
}

bool CXMLWriter::WriteEntity(const SXMLEntity &entity) {
    return DImplementation->WriteEntity(entity);
}

bool CXMLWriter::Flush() {
    return DImplementation->Flush();
}
