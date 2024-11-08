// =============================================================================
// @AUTHOR Vik Pandher

#include "OverTimeEntityDataFileReader.h"



namespace Project002
{
    OverTimeEntityDataFileReader::OverTimeEntityDataFileReader(const std::string& inputFileName,
        std::vector<std::string>& columnLabels,
        std::unordered_map<std::string, std::vector<std::string>>& dataMap,
        const unsigned int timeLabelColumn,
        const unsigned int entityIdColumn)
        : inputFileName_(inputFileName)
        , columnLabels_(columnLabels)
        , dataMap_(dataMap)
        , timeValueColumn_(timeLabelColumn)
        , entityIdColumn_(entityIdColumn)
        , lastTimeValue_(0.0)
        , noMoreData_(false)
    {}

    OverTimeEntityDataFileReader::~OverTimeEntityDataFileReader()
    {}

    bool OverTimeEntityDataFileReader::Initialize()
    {
        inputFileStream_.open(inputFileName_);
        if (!inputFileStream_.is_open())
        {
            // Can't read file
            return false;
        }

        std::string columnLabelLine;
        std::getline(inputFileStream_, columnLabelLine, '\n');
        if (columnLabelLine.empty())
        {
            // No column labels
            return false;
        }

        std::stringstream columnLabelLineStream(columnLabelLine);
        std::string currentLabel;
        while (std::getline(columnLabelLineStream, currentLabel, ','))
        {
            columnLabels_.push_back(currentLabel);
        }

        std::string dataLine;
        std::getline(inputFileStream_, dataLine, '\n');
        if (dataLine.empty())
        {
            // No data
            noMoreData_ = true;
        }

        if (!noMoreData_)
        {
            ParseDataLine(dataLine);
        }

        return true;
    }

    bool OverTimeEntityDataFileReader::Read(const double currentTime)
    {
        if (noMoreData_)
        {
            return false;
        }

        while (lastTimeValue_ <= currentTime)
        {
            dataMap_[lastEntityId_] = lastData_;

            lastData_.clear();

            std::string dataLine;
            std::getline(inputFileStream_, dataLine, '\n');
            if (dataLine.empty())
            {
                // No more data
                noMoreData_ = true;
                return true;
            }

            ParseDataLine(dataLine);
        }

        return true;
    }

    void OverTimeEntityDataFileReader::ParseDataLine(std::string dataLine)
    {
        std::stringstream dataLineStream(dataLine);
        std::string currentDatum;
        for (int i = 0; std::getline(dataLineStream, currentDatum, ','); ++i)
        {
            if (i == timeValueColumn_)
            {
                lastTimeValue_ = std::stod(currentDatum, nullptr);
            }
            else if (i == entityIdColumn_)
            {
                lastEntityId_ = currentDatum;
            }

            lastData_.push_back(currentDatum);
        }
    }
}