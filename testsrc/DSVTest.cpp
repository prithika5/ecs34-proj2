#include <gtest/gtest.h>

#include "DSVWriter.h"
#include "DSVReader.h"

#include "StringDataSink.h"
#include "StringDataSource.h"
// writer test

TEST(DSVWriterTest, EmptyRowTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({}));
    EXPECT_EQ(DataSink->String(),"\n");
}

TEST(DSVWriterTest, SingleRowTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_EQ(DataSink->String(),"");
    EXPECT_TRUE(Writer.WriteRow({"A","B","C"}));
    EXPECT_EQ(DataSink->String(),"A,B,C\n");
}

TEST(DSVWriterTest, DifferentDelimiterTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,'|');

    EXPECT_TRUE(DataSink->String().empty());
    EXPECT_TRUE(Writer.WriteRow({"A","B","C"}));
    EXPECT_EQ(DataSink->String(),"A|B|C\n");
}

TEST(DSVWriterTest, EmptyFieldTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(Writer.WriteRow({"A","","C"}));
    EXPECT_EQ(DataSink->String(),"A,,C\n");
}

TEST(DSVWriterTest, SingleFieldTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(Writer.WriteRow({"Hello"}));
    EXPECT_EQ(DataSink->String(),"Hello\n");
}

TEST(DSVWriterTest, MultipleWriteRowAppendTest){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink,',');

    EXPECT_TRUE(Writer.WriteRow({"A","B"}));
    EXPECT_EQ(DataSink->String(),"A,B\n");

    EXPECT_TRUE(Writer.WriteRow({"1","2"}));
    EXPECT_EQ(DataSink->String(),"A,B\n1,2\n");
}

TEST(DSVWriterTest, QuoteAllHitsAlwaysQuoteBranch){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink, ',', true); // quoteall = true

    EXPECT_TRUE(Writer.WriteRow({"A"}));
    EXPECT_EQ(DataSink->String(), "\"A\"\n");
}

TEST(DSVWriterTest, NeedsQuoteHitsDelimiterBranch){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink, ',');

    
    EXPECT_TRUE(Writer.WriteRow({"A,B"}));
    EXPECT_EQ(DataSink->String(), "\"A,B\"\n");
}

TEST(DSVWriterTest, NeedsQuoteHitsEscapeQuoteBranch){
    std::shared_ptr<CStringDataSink> DataSink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(DataSink, ',');

    
    EXPECT_TRUE(Writer.WriteRow({"He said \"hi\""}));
    EXPECT_EQ(DataSink->String(), "\"He said \"\"hi\"\"\"\n");
}


// reader test


TEST(DSVReaderTest, EmptyTest){
    auto src = std::make_shared<CStringDataSource>("");
    CDSVReader r(src, ',');

    std::vector<std::string> row;
    EXPECT_FALSE(r.ReadRow(row));
    EXPECT_TRUE(r.End());
    EXPECT_TRUE(row.empty());
}

TEST(DSVReaderTest, OneRowTest){
    auto src = std::make_shared<CStringDataSource>("X,Y,Z\n");
    CDSVReader r(src, ',');

    std::vector<std::string> row;
    EXPECT_TRUE(r.ReadRow(row));
    ASSERT_EQ(row.size(), 3u);
    EXPECT_EQ(row[0], "X");
    EXPECT_EQ(row[1], "Y");
    EXPECT_EQ(row[2], "Z");

    row.clear();
    EXPECT_FALSE(r.ReadRow(row));
}

TEST(DSVReaderTest, TwoRowTest){
    auto src = std::make_shared<CStringDataSource>("X,Y\n1,2\n");
    CDSVReader r(src, ',');

    std::vector<std::string> row;

    EXPECT_TRUE(r.ReadRow(row));
    ASSERT_EQ(row.size(), 2u);
    EXPECT_EQ(row[0], "X");
    EXPECT_EQ(row[1], "Y");

    row.clear();
    EXPECT_TRUE(r.ReadRow(row));
    ASSERT_EQ(row.size(), 2u);
    EXPECT_EQ(row[0], "1");
    EXPECT_EQ(row[1], "2");

    row.clear();
    EXPECT_FALSE(r.ReadRow(row));
}

TEST(DSVReaderTest, EmptyColTest){
    auto src = std::make_shared<CStringDataSource>("X,,Z\n");
    CDSVReader r(src, ',');

    std::vector<std::string> row;
    EXPECT_TRUE(r.ReadRow(row));
    ASSERT_EQ(row.size(), 3u);
    EXPECT_EQ(row[0], "X");
    EXPECT_EQ(row[1], "");
    EXPECT_EQ(row[2], "Z");
}

TEST(DSVReaderTest, OtherDelimTest){
    auto src = std::make_shared<CStringDataSource>("X|Y|Z\n");
    CDSVReader r(src, '|');

    std::vector<std::string> row;
    EXPECT_TRUE(r.ReadRow(row));
    ASSERT_EQ(row.size(), 3u);
    EXPECT_EQ(row[0], "X");
    EXPECT_EQ(row[1], "Y");
    EXPECT_EQ(row[2], "Z");
}

TEST(DSVReaderTest, EOFRow){
    std::shared_ptr<CStringDataSource> src = std::make_shared<CStringDataSource>("A,B");
    CDSVReader r(src, ',');
    std::vector<std::string> row;
    
    EXPECT_TRUE(r.ReadRow(row));
    EXPECT_EQ(row.size(), 2u);
    EXPECT_EQ(row[0], "A");
    EXPECT_EQ(row[1], "B");

    row.clear();
    EXPECT_FALSE(r.ReadRow(row));
}

TEST(DSVReaderTest, BreakOnNewlineCharacter){
    auto src = std::make_shared<CStringDataSource>("A,B,C\n");
    CDSVReader r(src, ',');
    std::vector<std::string> row;
    
    EXPECT_TRUE(r.ReadRow(row));
    ASSERT_EQ(row.size(), 3u);
    EXPECT_EQ(row[0], "A");
    EXPECT_EQ(row[1], "B");
    EXPECT_EQ(row[2], "C");
}


