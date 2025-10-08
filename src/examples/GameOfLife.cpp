//
// Created by julian on 9/22/25.
//

#include "GameOfLife.h"

#include <algorithm>
#include <ranges>
#include <unistd.h>

// todo: create separate thread for input handling
namespace GameOfLife {
    void GameOfLifeApp::run() {
        windowOpen(MAIN_WINDOW, 100, 100, 550, 300,
                   defaultMask, "Test Window 1");

        auto lastTime = std::chrono::high_resolution_clock::now();
        while (running) {
            handleAllQueuedEvents();

            if (keyIsPressed(XK_Escape) || !windowCheckOpen(MAIN_WINDOW)) break;
            if (keyIsPressed(XK_space)) {
                isPaused = !isPaused;
                windowScheduleRedraw(MAIN_WINDOW);
            }

            if (!isPaused) {
                const auto currentTime = std::chrono::high_resolution_clock::now();
                const auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).
                        count();

                if (deltaTime >= stepIntervalMs) {
                    gridStep();
                    lastTime = currentTime;
                }
            }

            usleep(16000); // ~60 FPS // todo: replace with proper timing mechanism
            windowProcessRedrawQueue();
        }
    }


    void GameOfLifeApp::handleButtonPress(XButtonEvent &event) {
        const auto winId = windowRawToId(event.window);
        if (!winId.has_value() || !windowCheckOpen(winId.value())) return;

        const auto attrs = windowGetAttributes(winId.value());
        const int gridX = event.x * gridWidth / attrs.width;
        const int gridY = event.y * gridHeight / attrs.height;

        if (gridX < 0 || gridX >= gridWidth || gridY < 0 || gridY >= gridHeight) return;

        grid[gridX][gridY] = !grid[gridX][gridY];
        windowScheduleRedraw(winId.value());
    }

    void GameOfLifeApp::gridStep() {
        static bool newGrid[gridWidth][gridHeight];
        for (int x = 0; x < gridWidth; ++x) {
            for (int y = 0; y < gridHeight; ++y) {
                int liveNeighbors = 0;
                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        if (dx == 0 && dy == 0) continue;

                        const int nx = x + dx;
                        const int ny = y + dy;
                        if (nx >= 0 && nx < gridWidth && ny >= 0 && ny < gridHeight)
                            liveNeighbors += grid[nx][ny] ? 1 : 0;
                    }
                }
                if (grid[x][y]) newGrid[x][y] = (liveNeighbors == 2 || liveNeighbors == 3);
                else newGrid[x][y] = (liveNeighbors == 3);
            }
        }

        std::copy_n(&newGrid[0][0], gridHeight * gridWidth, &grid[0][0]);
        windowScheduleRedraw(MAIN_WINDOW);
    }

    void GameOfLifeApp::handleExpose(XExposeEvent &event) {
        if (event.count != 0) return;
        const auto eventWinId = windowRawToId(event.window);
        if (!eventWinId.has_value() || !windowCheckOpen(eventWinId.value())) return;

        const auto winId = eventWinId.value();

        static const auto gray = colorCreate(32000, 32000, 32000);
        static const auto black = colorCreate(0, 0, 0);

        switch (winId) {
            case MAIN_WINDOW: {
                const auto attrs = windowGetAttributes(winId);
                const int cellWidth = attrs.width / gridWidth;
                const int cellHeight = attrs.height / gridHeight;

                for (int x = 0; x < gridWidth; ++x) {
                    for (int y = 0; y < gridHeight; ++y) {
                        if (grid[x][y])
                            drawRectangle(winId, black, x * cellWidth, y * cellHeight,
                                          cellWidth - 1, cellHeight - 1);
                    }
                }

                // Draw grid lines
                for (int x = 0; x <= gridWidth; ++x)
                    drawLine(winId, gray, x * cellWidth, 0, x * cellWidth, attrs.height - 1);

                for (int y = 0; y <= gridHeight; ++y)
                    drawLine(winId, gray, 0, y * cellHeight, attrs.width - 1, y * cellHeight);

                // Draw paused text
                if (isPaused) {
                    const static std::string text = "PAUSED";
                    drawText(winId, black, 10, 20, defaultFont, text);
                }
            }

            break;
            default: break;
        }
    }
}
