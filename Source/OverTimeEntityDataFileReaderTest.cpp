// =============================================================================
// @AUTHOR Vik Pandher

#include "OverTimeEntityDataFileReaderTest.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <Windows.h>

#include "OverTimeEntityDataFileReader.h"



namespace Project002
{
    struct NumericStringComparator
    {
        bool operator()(const std::string& a, const std::string& b) const
        {
            return std::stoi(a) < std::stoi(b);
        }
    };

    int OverTimeEntityDataFileReaderTest()
    {
        // Creating OverTimeEntityDataFileReader
        // ====================================================================== //

        std::vector<std::string> columnLabels;
        std::unordered_map<std::string, std::vector<std::string>> dataMap;
        unsigned int timeLabelColumn = 0;
        unsigned int entityIdColumn = 1;
        Project002::OverTimeEntityDataFileReader overTimeEntityDataFileReader(
            "../Files/Test02.csv",
            columnLabels,
            dataMap,
            timeLabelColumn,
            entityIdColumn
        );

        if (!overTimeEntityDataFileReader.Initialize())
        {
            std::cerr << "ERROR: failed to initialize OverTimeEntityDataFileReader";
            std::cerr << std::endl;
            return 1;
        }
        // ---------------------------------------------------------------------- //

        // Creating Console Screen Buffer
        // ====================================================================== //

        // original active screen buffer
        HANDLE standardOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        HANDLE newConsoleScreenBufferHandle = CreateConsoleScreenBuffer(
            GENERIC_WRITE,
            FILE_SHARE_WRITE,
            NULL,
            CONSOLE_TEXTMODE_BUFFER,
            NULL
        );

        if (standardOutputHandle == INVALID_HANDLE_VALUE)
        {
            std::cerr << "ERROR: GetStdHandle(...) failed";
            std::cerr << std::endl << GetLastError() << std::endl;
            return 2;
        }

        if (newConsoleScreenBufferHandle == INVALID_HANDLE_VALUE)
        {
            std::cerr << "ERROR: CreateConsoleScreenBuffer(...) failed";
            std::cerr << std::endl << GetLastError() << std::endl;
            return 3;
        }

        if (!SetConsoleActiveScreenBuffer(newConsoleScreenBufferHandle))
        {
            std::cerr << "ERROR: CreateConsoleScreenBuffer(...) failed";
            std::cerr << std::endl << GetLastError() << std::endl;
            return 4;
        }

        unsigned int longestLabelLength = 0;
        unsigned int numberOfColumnLabels = (unsigned int)columnLabels.size();
        for (unsigned int i = 0; i < numberOfColumnLabels; ++i)
        {
            unsigned int currentLabelLength = (unsigned int)columnLabels[i].size();
            if (currentLabelLength > longestLabelLength)
            {
                longestLabelLength = currentLabelLength;
            }
        }

        unsigned int elapsedTimeStringX = 1;
        unsigned int elapsedTimeStringY = 1;
        unsigned int entityCountStringX = 1;
        unsigned int entityCountStringY = 2;
        unsigned int labelsX = 1;
        unsigned int labelsY = 4;
        unsigned int dataX = 2 + longestLabelLength;
        unsigned int dataY = 4;
        unsigned int dataColumnWidth = 10;
        unsigned int dataColumnCount = 12; //

        unsigned int bufferHeight = numberOfColumnLabels + dataY;
        unsigned int bufferWidth = dataX + (dataColumnWidth + 1) * dataColumnCount;
        size_t bufferSize = (size_t)bufferHeight * bufferWidth;

        CHAR_INFO* consoleScreenBufferPtr = new CHAR_INFO[(size_t)bufferSize];

        SMALL_RECT consoleScreenBufferDestinationRectangle; // exclusive coordinates
        consoleScreenBufferDestinationRectangle.Top = 0;
        consoleScreenBufferDestinationRectangle.Left = 0;
        consoleScreenBufferDestinationRectangle.Bottom = bufferHeight + 1;
        consoleScreenBufferDestinationRectangle.Right = bufferWidth + 1;

        COORD consoleScreenBufferSize;
        consoleScreenBufferSize.X = bufferWidth;
        consoleScreenBufferSize.Y = bufferHeight;

        COORD consoleScreenBufferLocation;
        consoleScreenBufferLocation.X = 0;
        consoleScreenBufferLocation.Y = 0;
        // ---------------------------------------------------------------------- //

        // Main Loop (Data is read in and presented)
        // ====================================================================== //

        auto startTime = std::chrono::system_clock::now().time_since_epoch();
        auto startTimeMS = std::chrono::duration_cast<std::chrono::milliseconds>(startTime).count();
        double elapsedTimeDouble = 0.0;
        while (overTimeEntityDataFileReader.Read(elapsedTimeDouble))
        {
            // Clear the console screen buffer data
            CHAR_INFO clearValue = { ' ', 0 };
            for (unsigned int i = 0; i < bufferSize; ++i)
            {
                consoleScreenBufferPtr[i] = clearValue;
            }

            // Get the elapsed time
            auto currentTime = std::chrono::system_clock::now().time_since_epoch();
            auto currentTimeMS = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime).count();
            auto elapsedTimeMS = currentTimeMS - startTimeMS;
            elapsedTimeDouble = (double)elapsedTimeMS / 1000.0;
            std::string elapsedTimeString = "Elapsed Time: " + std::to_string(elapsedTimeDouble);
            // -DELETE- elapsedTimeString.resize(elapsedTimeString.size() - 3);
            for (unsigned int i = 0; i < elapsedTimeString.size(); ++i)
            {
                CHAR_INFO currentChar = {
                    (WCHAR)elapsedTimeString[i],
                    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
                };
                unsigned int bufferIndex = elapsedTimeStringX + i + elapsedTimeStringY * bufferWidth;
                if (bufferIndex < bufferSize)
                {
                    consoleScreenBufferPtr[elapsedTimeStringX + i + elapsedTimeStringY * bufferWidth] = currentChar;
                }
            }

            // Get the entity count
            size_t entityCount = dataMap.size();
            std::string entityCountString = "Entity Count: " + std::to_string(entityCount);
            for (unsigned int i = 0; i < entityCountString.size(); ++i)
            {
                CHAR_INFO currentChar = {
                    (WCHAR)entityCountString[i],
                    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
                };
                unsigned int bufferIndex = entityCountStringX + i + entityCountStringY * bufferWidth;
                if (bufferIndex < bufferSize)
                {
                    consoleScreenBufferPtr[entityCountStringX + i + entityCountStringY * bufferWidth] = currentChar;
                }
            }

            // Column labels
            for (unsigned int j = 0; j < columnLabels.size(); ++j)
            {
                std::string& currentColumnLabel = columnLabels[j];
                for (unsigned int i = 0; i < currentColumnLabel.size(); ++i)
                {
                    CHAR_INFO currentChar = {
                        (WCHAR)currentColumnLabel[i],
                        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
                    };
                    unsigned int bufferIndex = labelsX + i + (labelsY + j) * bufferWidth;
                    if (bufferIndex < bufferSize)
                    {
                        consoleScreenBufferPtr[bufferIndex] = currentChar;
                    }
                }
            }

            // Colums of data
            std::vector<const std::string*> keyPointers;
            for (auto itr = dataMap.begin(); itr != dataMap.end(); ++itr)
            {
                keyPointers.push_back(&(itr->first));
            }

            // Sort the keys
            std::sort(
                keyPointers.begin(),
                keyPointers.end(),
                [](const std::string* a, const std::string* b)
                {
                    return std::stoi(*a) < std::stoi(*b);
                }
            );

            unsigned int k = 0;
            for (unsigned int i = 0; i < keyPointers.size() && k < dataColumnCount; ++i)
            {
                const std::vector<std::string>& data = dataMap.at(*keyPointers[i]);
                for (unsigned int j = 0; j < data.size(); ++j)
                {
                    const std::string& datum = data[j];
                    for (unsigned int i = 0; i < datum.size() && i < dataColumnWidth; ++i)
                    {
                        CHAR_INFO currentChar = {
                        (WCHAR)datum[i],
                        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
                        };
                        unsigned int bufferIndex = dataX + i + k * (dataColumnWidth + 1) + (dataY + j) * bufferWidth;
                        if (bufferIndex < bufferSize)
                        {
                            consoleScreenBufferPtr[bufferIndex] = currentChar;
                        }
                    }
                }
                ++k;
            }

            // Draw
            BOOL writeSuccessful = WriteConsoleOutput(
                newConsoleScreenBufferHandle,
                consoleScreenBufferPtr,
                consoleScreenBufferSize,
                consoleScreenBufferLocation,
                &consoleScreenBufferDestinationRectangle
            );
            if (!writeSuccessful)
            {
                std::cerr << "ERROR: WriteConsoleOutput(...) failed";
                std::cerr << std::endl << GetLastError() << std::endl;
                return 5;
            }
        }

        // Wait for any key press
        std::cin.get();

        // ---------------------------------------------------------------------- //

        // Cleanup
        // ====================================================================== //

        if (!SetConsoleActiveScreenBuffer(standardOutputHandle))
        {
            std::cerr << "ERROR: SetConsoleActiveScreenBuffer(...) failed";
            std::cerr << std::endl << GetLastError() << std::endl;
            return 6;
        }
        // ---------------------------------------------------------------------- //

        return 0;
    }
}