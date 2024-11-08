// =============================================================================
// @AUTHOR Vik Pandher

#pragma once

#include <fstream>
#include <unordered_map>
#include <sstream>
#include <vector>



namespace Project002
{
    class OverTimeEntityDataFileReader
    {
    public:
        OverTimeEntityDataFileReader(const std::string& inputFileName,
            std::vector<std::string>& columnLabels,
            std::unordered_map<std::string, std::vector<std::string>>& dataMap,
            const unsigned int timeLabelColumn,
            const unsigned int entityIdColumn);
        ~OverTimeEntityDataFileReader();

        bool Initialize();

        bool Read(const double currentTime);

    protected:
        void ParseDataLine(std::string dataLine);

        std::ifstream inputFileStream_;

        std::string inputFileName_;
        std::vector<std::string>& columnLabels_;
        unsigned int timeValueColumn_;
        unsigned int entityIdColumn_;
        std::unordered_map<std::string, std::vector<std::string>>& dataMap_;

        double lastTimeValue_;
        std::string lastEntityId_;
        std::vector<std::string> lastData_;

        bool noMoreData_;

    private:
    };
}