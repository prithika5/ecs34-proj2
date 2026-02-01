#include <gtest/gtest.h>
#include "XMLWriter.h"
#include "XMLReader.h"
#include "StringDataSource.h"
#include "StringDataSink.h"
#include "XMLEntity.h"


// XML Writer tests

TEST(XMLWrite, Empty){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    EXPECT_TRUE(Sink->String().empty());
    EXPECT_EQ(Sink->String(),"");
    EXPECT_TRUE(W.Flush());
    EXPECT_EQ(Sink->String(),"");
}

TEST(XMLWrite, One){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::CompleteElement;
    E.DNameData = "hello";
    EXPECT_TRUE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"<hello/>");
}

TEST(XMLWrite, Pair){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity A;
    SXMLEntity B;
    A.DType = SXMLEntity::EType::StartElement;
    A.DNameData = "r";
    B.DType = SXMLEntity::EType::EndElement;
    B.DNameData = "r";
    EXPECT_TRUE(W.WriteEntity(A));
    EXPECT_TRUE(W.WriteEntity(B));
    EXPECT_EQ(Sink->String(),"<r></r>");
}

TEST(XMLWrite, Nest){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity A;
    SXMLEntity B;
    A.DType = SXMLEntity::EType::StartElement;
    A.DNameData = "a";
    B.DType = SXMLEntity::EType::StartElement;
    B.DNameData = "b";
    EXPECT_TRUE(W.WriteEntity(A));
    EXPECT_TRUE(W.WriteEntity(B));
    EXPECT_TRUE(W.Flush());
    EXPECT_EQ(Sink->String(),"<a><b></b></a>");
}

TEST(XMLWrite, Text){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::CharData;
    E.DNameData = "1&2<3>4";
    EXPECT_TRUE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"1&amp;2&lt;3&gt;4");
}

TEST(XMLWrite, Plain){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::CharData;
    E.DNameData = "abcd";
    EXPECT_TRUE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"abcd");
}

TEST(XMLWrite, Attr){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::CompleteElement;
    E.DNameData = "x";
    E.DAttributes.push_back(std::make_pair(std::string("a"), std::string("Hello&\"<t>")));
    E.DAttributes.push_back(std::make_pair(std::string("b"), std::string("ok")));
    EXPECT_TRUE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"<x a=\"Hello&amp;&quot;&lt;t&gt;\" b=\"ok\"/>");
}

TEST(XMLWrite, Bad1){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::StartElement;
    E.DNameData = "";
    EXPECT_FALSE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"");
}

TEST(XMLWrite, Bad2){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::CompleteElement;
    E.DNameData = "";
    EXPECT_FALSE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"");
}

TEST(XMLWrite, Bad3){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::EndElement;
    E.DNameData = "";
    EXPECT_FALSE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"");
}

TEST(XMLWrite, Bad4){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::EndElement;
    E.DNameData = "a";
    EXPECT_FALSE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"");
}

TEST(XMLWrite, Bad5){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity A;
    A.DType = SXMLEntity::EType::StartElement;
    A.DNameData = "a";
    SXMLEntity B;
    B.DType = SXMLEntity::EType::EndElement;
    B.DNameData = "b";
    EXPECT_TRUE(W.WriteEntity(A));
    EXPECT_FALSE(W.WriteEntity(B));
}

TEST(XMLWrite, Other){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = static_cast<SXMLEntity::EType>(999);
    E.DNameData = "x";
    EXPECT_TRUE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"");
}

TEST(XMLWrite, Null){
    std::shared_ptr<CDataSink> Sink;
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::CompleteElement;
    E.DNameData = "a";
    EXPECT_FALSE(W.WriteEntity(E));
}

TEST(XMLWrite, Entity){
    SXMLEntity E;
    EXPECT_FALSE(E.AttributeExists("a"));
    EXPECT_EQ(E.AttributeValue("a"),"");
    EXPECT_FALSE(E.SetAttribute("", "1"));
    EXPECT_TRUE(E.SetAttribute("a", "1"));
    EXPECT_TRUE(E.AttributeExists("a"));
    EXPECT_EQ(E.AttributeValue("a"),"1");
    EXPECT_TRUE(E.SetAttribute("a", "2"));
    EXPECT_EQ(E.AttributeValue("a"),"2");
}

// XML Reader tests

TEST(XMLRead, Tag){
    std::string t = "<a></a>";
    std::shared_ptr<CStringDataSource> s = std::make_shared<CStringDataSource>(t);
    CXMLReader r(s);
    SXMLEntity e;
    EXPECT_TRUE(r.ReadEntity(e));
    EXPECT_EQ(e.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(e.DNameData, "a");
}

TEST(XMLRead, End){
    std::string t = "<a></a>";
    std::shared_ptr<CStringDataSource> s = std::make_shared<CStringDataSource>(t);
    CXMLReader r(s);
    SXMLEntity e;
    EXPECT_TRUE(r.ReadEntity(e));
    EXPECT_TRUE(r.ReadEntity(e));
    EXPECT_EQ(e.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(e.DNameData, "a");
}

TEST(XMLRead, Txt){
    std::string t = "<a>hi</a>";
    std::shared_ptr<CStringDataSource> s = std::make_shared<CStringDataSource>(t);
    CXMLReader r(s);
    SXMLEntity e;
    EXPECT_TRUE(r.ReadEntity(e));
    EXPECT_TRUE(r.ReadEntity(e));
    EXPECT_EQ(e.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(e.DNameData, "hi");
}

TEST(XMLRead, Skip){
    std::string t = "<a>hi</a>";
    std::shared_ptr<CStringDataSource> s = std::make_shared<CStringDataSource>(t);
    CXMLReader r(s);
    SXMLEntity e;
    EXPECT_TRUE(r.ReadEntity(e, true));
    EXPECT_EQ(e.DType, SXMLEntity::EType::StartElement);
    EXPECT_TRUE(r.ReadEntity(e, true));
    EXPECT_EQ(e.DType, SXMLEntity::EType::EndElement);
}

TEST(XMLRead, Att){
    std::string t = "<a id=\"7\"></a>";
    std::shared_ptr<CStringDataSource> s = std::make_shared<CStringDataSource>(t);
    CXMLReader r(s);
    SXMLEntity e;
    EXPECT_TRUE(r.ReadEntity(e));
    EXPECT_TRUE(e.AttributeExists("id"));
    EXPECT_EQ(e.AttributeValue("id"), "7");
}

TEST(XMLRead, Att2){
    std::string t = "<a x=\"1\" y=\"2\"></a>";
    std::shared_ptr<CStringDataSource> s = std::make_shared<CStringDataSource>(t);
    CXMLReader r(s);
    SXMLEntity e;
    EXPECT_TRUE(r.ReadEntity(e));
    EXPECT_EQ(e.AttributeValue("x"), "1");
    EXPECT_EQ(e.AttributeValue("y"), "2");
}

TEST(XMLRead, Self){
    std::string t = "<a/>";
    std::shared_ptr<CStringDataSource> s = std::make_shared<CStringDataSource>(t);
    CXMLReader r(s);
    SXMLEntity e;
    EXPECT_TRUE(r.ReadEntity(e));
    EXPECT_EQ(e.DType, SXMLEntity::EType::CompleteElement);
    EXPECT_EQ(e.DNameData, "a");
}

TEST(XMLRead, Two){
    std::string t = "<a></a><b></b>";
    std::shared_ptr<CStringDataSource> s = std::make_shared<CStringDataSource>(t);
    CXMLReader r(s);
    SXMLEntity e;
    EXPECT_TRUE(r.ReadEntity(e));
    EXPECT_TRUE(r.ReadEntity(e));
    EXPECT_TRUE(r.ReadEntity(e));
    EXPECT_EQ(e.DNameData, "b");
}

TEST(XMLRead, Done){
    std::string t = "<a></a>";
    std::shared_ptr<CStringDataSource> s = std::make_shared<CStringDataSource>(t);
    CXMLReader r(s);
    SXMLEntity e;
    EXPECT_TRUE(r.ReadEntity(e));
    EXPECT_TRUE(r.ReadEntity(e));
    EXPECT_FALSE(r.ReadEntity(e));
    EXPECT_TRUE(r.End());
}

TEST(XMLRead, Spc){
    std::string t = "<a>  hi  </a>";
    std::shared_ptr<CStringDataSource> s = std::make_shared<CStringDataSource>(t);
    CXMLReader r(s);
    SXMLEntity e;
    EXPECT_TRUE(r.ReadEntity(e));
    EXPECT_TRUE(r.ReadEntity(e));
    EXPECT_EQ(e.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(e.DNameData, "  hi  ");
}