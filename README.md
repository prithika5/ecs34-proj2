# Project X README

Student 1: Prithika Thilakarajan (923266507)

Student 2: Qi Gao (123456987)

## Project Status
The project is complete and builds successfully. All required functionality for DSV reading/writing and XML reading/writing has been implemented, and all provided tests pass. Code coverage requirements have also been met.

## Known Issues
There are no known functional issues at this time.

## Code References
* [C++ Reference](https://en.cppreference.com/)
* [CPlusPlus Documentation](https://cplusplus.com/)
* [Expat XML Parser](https://libexpat.github.io/)
* [Git Documentation](https://git-scm.com/docs)
* [GNU Make Manual](https://www.gnu.org/software/make/manual/make.html)

## Generative AI Use

### Prompt 1
    How do I use Expat to parse XML that doesn’t have one root tag, and then just ignore the fake <root> I add?
### Response 1
    You can wrap the raw XML text in a temporary root tag before calling XML_Parse, then skip that wrapper element when returning entities.
    Example:
    std::string Wrap = "<root>";
    Wrap += Text;
    Wrap += "</root>";
    XML_Parser x = XML_ParserCreate(NULL);
    XML_SetUserData(x, this);
    XML_SetElementHandler(x, Start, End);
    XML_SetCharacterDataHandler(x, Char);
    XML_Parse(x, Wrap.c_str(), (int)Wrap.size(), 1);
    XML_ParserFree(x);
    Then, when reading entities back out:
    if (e.DNameData == "root") {
        if (e.DType == SXMLEntity::EType::StartElement) continue;
        if (e.DType == SXMLEntity::EType::EndElement) continue;
    }
## Changes 1
    I used this idea and implemented it in my own code by wrapping the input with <root>...</root> and then skipping "root" start/end entities in ReadEntity.



### Prompt 2
    How do I take what Expat gives me (start tag, end tag, text, attributes) and save it into my own struct/list while it parses?
### Response 2
    In Expat, you register callback handlers. In each callback you can create your own event object and push it into a list. For attributes, Expat provides a null-terminated array of key/value pairs.
    Example:
    static void Start(void *data, const XML_Char *name, const XML_Char **ats) {
    auto *self = (SImplementation*)data;
    SXMLEntity e;
    e.DType = SXMLEntity::EType::StartElement;
    e.DNameData = std::string(name);
    int i = 0;
    while (ats && ats[i]) {
        std::string k = std::string(ats[i]);
        std::string v = ats[i + 1] ? std::string(ats[i + 1]) : "";
        e.DAttributes.push_back(std::make_pair(k, v));
        i += 2;
    }
    self->List.push_back(e);
}
    static void End(void *data, const XML_Char *name) {
    auto *self = (SImplementation*)data;
    SXMLEntity e;
    e.DType = SXMLEntity::EType::EndElement;
    e.DNameData = std::string(name);
    self->List.push_back(e);
    }
    static void Char(void *data, const XML_Char *s, int len) {
    auto *self = (SImplementation*)data;
    SXMLEntity e;
    e.DType = SXMLEntity::EType::CharData;
    e.DNameData = std::string(s, s + len);
    self->List.push_back(e);}
## Changes 2
    I implemented these handlers in my own project to convert Expat events into SXMLEntity objects and store them in List, including writing my own loop to extract attribute key/value pairs and my own ReadEntity function to iterate through stored entities (including skipcdata behavior).



## Prompt 3
    How do I write XML entities (start tag, end tag, text, complete tag) to a `CDataSink` in C++?
## Response 3
    You can write a `WriteEntity(const SXMLEntity&)` function that checks the entity type and outputs the correct XML text to the sink. 
    bool CXMLWriter::WriteEntity(const SXMLEntity &entity){
    if(entity.DType == SXMLEntity::EType::StartElement){
        // write: <tag ...>
    }else if(entity.DType == SXMLEntity::EType::EndElement){
        // write: </tag>
    }else if(entity.DType == SXMLEntity::EType::CharData){
        // write: text
    }else if(entity.DType == SXMLEntity::EType::CompleteElement){
        // write: <tag .../>
    }
    return true;}
## Changes 3
    I used this as guidance and implemented my own `WriteEntity` that formats output based on `SXMLEntity::EType` and writes it through my `CDataSink`.


    
## Prompt 4
    How do I implement a Flush() method for a writer class that outputs to a sink?
## Response 4
    A `Flush()` method typically forwards the flush request to the underlying sink (if supported) or ensures any buffered output is written.
    Example:        
    bool CXMLWriter::Flush(){
    // if your sink buffers output, flush it here
    return true; }
## Changes 4
    I decided how flushing should work for my `CDataSink` design and implemented `Flush()` accordingly based on whether my sink buffers output.

## Prompt 5
    How do I write unit tests for my XMLWriter using GoogleTest and a string-based sink?
## Response 5
    You can use a CStringDataSink to capture the output of the writer and then compare the resulting string against the expected XML using EXPECT_EQ.
    Example:
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::CompleteElement;
    E.DNameData = "hello";
    W.WriteEntity(E);
    EXPECT_EQ(Sink->String(), "<hello/>");
## Changes 5
    I implemented multiple GoogleTest cases using CStringDataSink to verify correct XML output for empty input, single elements, nested elements, attributes, escaped text, and invalid entities.


    
## Prompt 6
How do I write unit tests for my XMLReader using GoogleTest and a string-based source?
## Response 6
You can store XML text in a CStringDataSource, call ReadEntity, and verify the returned SXMLEntity fields such as type, name, text, and attributes using GoogleTest assertions.
    Example:
    std::string t = "<a>hi</a>";
    std::shared_ptr<CStringDataSource> s = std::make_shared<CStringDataSource>(t);
    CXMLReader r(s);
    SXMLEntity e;
    r.ReadEntity(e);
    r.ReadEntity(e);
    EXPECT_EQ(e.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(e.DNameData, "hi");
## Changes 6
    I implemented my own XMLReader tests to verify parsing behavior for start and end tags, character data, attribute handling, self closing elements, skipping character data, multiple top level elements, and end of input conditions.

    

