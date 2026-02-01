#include "XMLReader.h"
#include <string>
#include <vector>
#include <expat.h>

struct CXMLReader::SImplementation{

    std::shared_ptr<CDataSource> Src;
    std::string Text;
    std::vector<SXMLEntity> List;
    int Pos;
    std::string CompleteName;

    SImplementation(std::shared_ptr<CDataSource> src){
        Src = src;
        Pos = 0;
        CompleteName = "";

        
        if(Src != nullptr){
            char c;
            while(Src->Get(c)){
                Text.push_back(c);
            }
        }

        
        std::string Wrap = "<root>";;
        Wrap += Text;
        Wrap += "</root>";

       
        XML_Parser x = XML_ParserCreate(NULL);
        XML_SetUserData(x, this);
        XML_SetElementHandler(x, Start, End);
        XML_SetCharacterDataHandler(x, Char);
        XML_Parse(x, Wrap.c_str(), (int)Wrap.size(), 1);
        XML_ParserFree(x);
    }

    static void Start(void *data, const XML_Char *name, const XML_Char **ats){
        SImplementation *self = (SImplementation*)data;
        std::string s = std::string(name);
        std::string tag = "<";
        tag += s;
        tag += "/>";

        if(self->Text.find(tag) != std::string::npos){
            self->CompleteName = s;
        }
        else{
            self->CompleteName = "";
        }

        SXMLEntity e;
        e.DType = SXMLEntity::EType::StartElement;
        e.DNameData = s;

        int i = 0;
        while(ats && ats[i]){
            std::string k = std::string(ats[i]);
            std::string v = "";
            if(ats[i + 1]){
                v = std::string(ats[i + 1]);
            }
            e.DAttributes.push_back(std::make_pair(k, v));
            i = i + 2;
        }

        self->List.push_back(e);
    }

    static void End(void *data, const XML_Char *name){
        SImplementation *self = (SImplementation*)data;
        std::string n = std::string(name);

       
        if(self->CompleteName == n){
            if(self->List.size() > 0){
                SXMLEntity &last = self->List[self->List.size() - 1];
                if(last.DType == SXMLEntity::EType::StartElement &&
                   last.DNameData == n){
                    last.DType = SXMLEntity::EType::CompleteElement;
                    return;
                }
            }
        }

        SXMLEntity e;
        e.DType = SXMLEntity::EType::EndElement;
        e.DNameData = n;
        self->List.push_back(e);
    }

    static void Char(void *data, const XML_Char *s, int len){
        SImplementation *self = (SImplementation*)data;

        SXMLEntity e;
        e.DType = SXMLEntity::EType::CharData;
        e.DNameData = std::string(s, s + len);
        self->List.push_back(e);
    }

    bool End() const{
        return Pos >= (int)List.size();
    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata){
        while(Pos < (int)List.size()){
            SXMLEntity e = List[Pos];
            Pos = Pos + 1;


            if(e.DNameData == "root"){
                if(e.DType == SXMLEntity::EType::StartElement){
                    continue;
                }
                if(e.DType == SXMLEntity::EType::EndElement){
                    continue;
                }
            }
            if(skipcdata && e.DType == SXMLEntity::EType::CharData){
                continue;
            }

            entity = e;
            return true;
        }
        return false;
    }
};

CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src){
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
