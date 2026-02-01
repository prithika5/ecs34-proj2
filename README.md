# Project 2 README

## Team Members
- **Student 1:** Prithika Thilakarajan (923266507)
- **Student 2:** Qi Gao (921902627)

## Project Status

The project is complete and builds successfully. All required functionality for DSV reading/writing and XML reading/writing has been implemented, and all provided tests pass. Code coverage requirements have also been met.

## Known Issues

There are no known functional issues at this time.

## References

- [C++ Reference](https://en.cppreference.com/)
- [CPlusPlus Documentation](https://cplusplus.com/)
- [Expat XML Parser](https://libexpat.github.io/)
- [Git Documentation](https://git-scm.com/docs)
- [GNU Make Manual](https://www.gnu.org/software/make/manual/make.html)

## Generative AI Use

The following prompts and responses were used to guide implementation:

### Student 1 (Prithika) - XML Implementation Prompts

#### Prompt 1: XML Parsing Without Root Tag

**Question:** How do I use Expat to parse XML that doesn't have one root tag, and then just ignore the fake <root> I add?

**Response:** You can wrap the raw XML text in a temporary root tag before calling XML_Parse, then skip that wrapper element when returning entities.

```cpp
std::string Wrap = "<root>";
Wrap += Text;
Wrap += "</root>";
XML_Parser x = XML_ParserCreate(NULL);
XML_SetUserData(x, this);
XML_SetElementHandler(x, Start, End);
XML_SetCharacterDataHandler(x, Char);
XML_Parse(x, Wrap.c_str(), (int)Wrap.size(), 1);
XML_ParserFree(x);
```

Then, when reading entities back out:
```cpp
if (e.DNameData == "root") {
    if (e.DType == SXMLEntity::EType::StartElement) continue;
    if (e.DType == SXMLEntity::EType::EndElement) continue;
}
```

**Implementation:** Used this approach and implemented it by wrapping the input with `<root>` tags and skipping "root" start/end entities in `ReadEntity`.

---

#### Prompt 2: Saving Expat Events to Structs

**Question:** How do I take what Expat gives me (start tag, end tag, text, attributes) and save it into my own struct/list while it parses?

**Response:** In Expat, you register callback handlers. In each callback you can create your own event object and push it into a list. For attributes, Expat provides a null-terminated array of key/value pairs.

```cpp
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
    self->List.push_back(e);
}
```

**Implementation:** Implemented these handlers to convert Expat events into `SXMLEntity` objects and store them in a list, including extracting attribute key/value pairs and creating a `ReadEntity` function to iterate through stored entities.

---

#### Prompt 3: Writing XML Entities to CDataSink

**Question:** How do I write XML entities (start tag, end tag, text, complete tag) to a `CDataSink` in C++?

**Response:** You can write a `WriteEntity(const SXMLEntity&)` function that checks the entity type and outputs the correct XML text to the sink.

```cpp
bool CXMLWriter::WriteEntity(const SXMLEntity &entity){
    if(entity.DType == SXMLEntity::EType::StartElement){
        // write: <name attr="value">
    }else if(entity.DType == SXMLEntity::EType::EndElement){
        // write: </name>
    }else if(entity.DType == SXMLEntity::EType::CharData){
        // write: text
    }else if(entity.DType == SXMLEntity::EType::CompleteElement){
        // write: <name attr="value"/>
    }
    return true;
}
```

**Implementation:** Used this guidance to implement a `WriteEntity` function that formats output based on `SXMLEntity::EType` and writes it through the `CDataSink`.

---

#### Prompt 4: Implementing Flush() Method

**Question:** How do I implement a Flush() method for a writer class that outputs to a sink?

**Response:** A `Flush()` method typically forwards the flush request to the underlying sink (if supported) or ensures any buffered output is written.

```cpp
bool CXMLWriter::Flush(){
    // if your sink buffers output, flush it here
    return true;
}
```

**Implementation:** Decided how flushing should work for the `CDataSink` design and implemented `Flush()` accordingly based on whether the sink buffers output.

---

#### Prompt 5: Unit Testing XMLWriter with GoogleTest

**Question:** How do I write unit tests for my XMLWriter using GoogleTest and a string-based sink?

**Response:** You can use a `CStringDataSink` to capture the output of the writer and then compare the resulting string against the expected XML using `EXPECT_EQ`.

```cpp
std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
CXMLWriter W(Sink);
SXMLEntity E;
E.DType = SXMLEntity::EType::CompleteElement;
E.DNameData = "hello";
W.WriteEntity(E);
EXPECT_EQ(Sink->String(), "<hello/>");
```

**Implementation:** Implemented multiple GoogleTest cases using `CStringDataSink` to verify correct XML output for empty input, single elements, nested elements, attributes, escaped text, and invalid entities.

---

#### Prompt 6: Unit Testing XMLReader with GoogleTest

**Question:** How do I write unit tests for my XMLReader using GoogleTest and a string-based source?

**Response:** You can store XML text in a `CStringDataSource`, call `ReadEntity`, and verify the returned `SXMLEntity` fields such as type, name, text, and attributes using GoogleTest assertions.

```cpp
std::string t = "<hello>hi</hello>";
std::shared_ptr<CStringDataSource> s = std::make_shared<CStringDataSource>(t);
CXMLReader r(s);
SXMLEntity e;
r.ReadEntity(e);
r.ReadEntity(e);
EXPECT_EQ(e.DType, SXMLEntity::EType::CharData);
EXPECT_EQ(e.DNameData, "hi");
```

**Implementation:** Implemented XMLReader tests to verify parsing behavior for start and end tags, character data, attribute handling, self-closing elements, skipping character data, multiple top-level elements, and end of input conditions.

---

### Student 2 (Qi Gao) - DSV Implementation Prompts

#### Prompt 1: DSVWriter Test Failure with Newlines

**Question:** Why is my DSVWriter unit test failing even though the output looks correct?

**Response:** Most DSV/CSV writers append a newline at the end of each row. If your test expects "A,B,C" but the writer outputs "A,B,C\n", the string comparison will fail. Fix: update expected strings in tests to include \n at the end of each WriteRow call.

```cpp
EXPECT_TRUE(w.WriteRow({"A","B","C"}));
EXPECT_EQ(sink->String(), "A,B,C\n");
```

**Implementation:** Updated DSVWriter tests to match the writer behavior by expecting a newline at the end of every written row, including the empty row case (expected output becomes just "\n").

---

#### Prompt 2: Verifying DSVWriter Multi-Row Output

**Question:** How can I verify that my DSVWriter correctly appends multiple rows instead of overwriting output?

**Response:** Use a CStringDataSink to capture output, call WriteRow multiple times, and compare the final string. This ensures the writer appends rows correctly and includes delimiters and newlines for each call.

```cpp
w.WriteRow({"A","B"});
w.WriteRow({"1","2"});
EXPECT_EQ(sink->String(), "A,B\n1,2\n");
```

**Implementation:** Added a unit test that writes multiple rows sequentially and verifies that each row is appended with a newline, ensuring no data is overwritten between calls.

---

#### Prompt 3: Implementing Simple DSVReader

**Question:** How do I implement a simple DSVReader that reads rows correctly from a CDataSource?

**Response:** A simple DSVReader can read characters one at a time until it encounters a newline (\n) or reaches the end of the source. The collected line can then be split using the delimiter. Ignoring carriage returns (\r) helps support Windows-style line endings.

Basic steps:
- Return false if the source is already at end
- Read characters using Get
- Stop at \n
- Ignore \r
- Split the line by the delimiter

```cpp
bool CDSVReader::ReadRow(std::vector<std::string> &row){
    row.clear();
    
    if(DImplementation->Src->End()){
        return false;
    }
    
    std::string line = "";
    char c;
    
    while(!DImplementation->Src->End()){
        if(!DImplementation->Src->Get(c)){
            break;
        }
        
        if(c == '\n'){
            break;
        }
        
        if(c != '\r'){
            line += c;
        }
    }
    
    row = StringUtils::Split(line, std::string(1, DImplementation->Delim));
    return true;
}
```

**Implementation:** Implemented CDSVReader::ReadRow to read input character-by-character, stop on newline, ignore carriage returns, and split the line using StringUtils::Split to produce the output vector.

---

#### Prompt 4: Unit Testing DSVReader with GoogleTest

**Question:** How do I write unit tests for my DSVReader using GoogleTest?

**Response:** Store DSV-formatted text in a CStringDataSource, repeatedly call ReadRow, and verify both the returned row contents and the function's return value. Tests should cover empty input, multiple rows, empty fields, and alternate delimiters.

```cpp
auto src = std::make_shared<CStringDataSource>("X,Y\n1,2\n");
CDSVReader r(src, ',');
std::vector<std::string> row;

EXPECT_TRUE(r.ReadRow(row));  // {"X","Y"}
EXPECT_TRUE(r.ReadRow(row));  // {"1","2"}
EXPECT_FALSE(r.ReadRow(row)); // end of input
```

**Implementation:** Added GoogleTest cases for empty input, single-row input, multiple rows, empty columns, and non-comma delimiters to confirm that ReadRow behaves correctly and stops at end-of-input.

---


