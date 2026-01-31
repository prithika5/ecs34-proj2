#include <gtest/gtest.h>
#include "XMLWriter.h"
#include "StringDataSink.h"
#include "XMLEntity.h"

TEST(XMLTest, Empty){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    EXPECT_TRUE(Sink->String().empty());
    EXPECT_EQ(Sink->String(),"");
    EXPECT_TRUE(W.Flush());
    EXPECT_EQ(Sink->String(),"");
}

TEST(XMLTest, One){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::CompleteElement;
    E.DNameData = "hello";
    EXPECT_TRUE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"<hello/>");
}

TEST(XMLTest, Pair){
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

TEST(XMLTest, Nest){
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

TEST(XMLTest, Text){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::CharData;
    E.DNameData = "1&2<3>4";
    EXPECT_TRUE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"1&amp;2&lt;3&gt;4");
}

TEST(XMLTest, Plain){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::CharData;
    E.DNameData = "abcd";
    EXPECT_TRUE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"abcd");
}

TEST(XMLTest, Attr){
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

TEST(XMLTest, Bad1){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::StartElement;
    E.DNameData = "";
    EXPECT_FALSE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"");
}

TEST(XMLTest, Bad2){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::CompleteElement;
    E.DNameData = "";
    EXPECT_FALSE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"");
}

TEST(XMLTest, Bad3){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::EndElement;
    E.DNameData = "";
    EXPECT_FALSE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"");
}

TEST(XMLTest, Bad4){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::EndElement;
    E.DNameData = "a";
    EXPECT_FALSE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"");
}

TEST(XMLTest, Bad5){
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

TEST(XMLTest, Other){
    std::shared_ptr<CStringDataSink> Sink = std::make_shared<CStringDataSink>();
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = static_cast<SXMLEntity::EType>(999);
    E.DNameData = "x";
    EXPECT_TRUE(W.WriteEntity(E));
    EXPECT_EQ(Sink->String(),"");
}

TEST(XMLTest, Null){
    std::shared_ptr<CDataSink> Sink;
    CXMLWriter W(Sink);
    SXMLEntity E;
    E.DType = SXMLEntity::EType::CompleteElement;
    E.DNameData = "a";
    EXPECT_FALSE(W.WriteEntity(E));
}

TEST(XMLTest, Entity){
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
