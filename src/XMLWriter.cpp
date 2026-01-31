#include "XMLWriter.h"
#include <memory>
#include <vector>
#include <string>

struct CXMLWriter::SImplementation {

    std::shared_ptr<CDataSink> Sink;
    std::vector<std::string> Open;

    SImplementation(std::shared_ptr<CDataSink> sink) {
        Sink = sink;
    }

    ~SImplementation() {
    }

    bool PutStr(const std::string &s) {
        std::vector<char> buf(s.begin(), s.end());
        return Sink->Write(buf);
    }

    std::string EscText(const std::string &s) {
        std::string out;
        for (char c : s) {
            if (c == '&') out += "&amp;";
            else if (c == '<') out += "&lt;";
            else if (c == '>') out += "&gt;";
            else out += c;
        }
        return out;
    }

    std::string EscAttr(const std::string &s) {
        std::string out;
        for (char c : s) {
            if (c == '&') out += "&amp;";
            else if (c == '<') out += "&lt;";
            else if (c == '>') out += "&gt;";
            else if (c == '"') out += "&quot;";
            else out += c;
        }
        return out;
    }

    bool PutAttr(const TAttributes &attrs) {
        for (TAttributes::size_type i = 0; i < attrs.size(); i++) {
            const std::string &k = attrs[i].first;
            const std::string &v = attrs[i].second;

            PutStr(" ");
            PutStr(k);
            PutStr("=\"");
            PutStr(EscAttr(v));
            PutStr("\"");
        }
        return true;
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
