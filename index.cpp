#include <iostream>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <limits>

using namespace std;

/*==========================================================
                        NODE CLASS
  Each node represents one page currently present in RAM.
  The nodes are connected through a Doubly Linked List.
==========================================================*/

class Node {
public:
    int pageNumber;
    Node* previous;
    Node* next;

    Node(int page) {
        pageNumber = page;
        previous = nullptr;
        next = nullptr;
    }
};

/*==========================================================
                     SIMULATION RESULT
==========================================================*/

struct SimulationResult {
    int hits = 0;
    int faults = 0;

    double getHitRatio() const {
        int totalRequests = hits + faults;

        if (totalRequests == 0) {
            return 0.0;
        }

        return (static_cast<double>(hits) / totalRequests) * 100.0;
    }

    double getFaultRatio() const {
        int totalRequests = hits + faults;

        if (totalRequests == 0) {
            return 0.0;
        }

        return (static_cast<double>(faults) / totalRequests) * 100.0;
    }
};

/*==========================================================
                         LRU CACHE
   DSA used:
   1. Hash Map for O(1) page lookup
   2. Doubly Linked List for O(1) insertion and deletion

   Head = Most Recently Used page
   Tail = Least Recently Used page
==========================================================*/

class LRUCache {
private:
    int capacity;
    int currentSize;

    Node* head;
    Node* tail;

    unordered_map<int, Node*> pageMap;

    // Adds a new node at the beginning of the list.
    void addToFront(Node* node) {
        node->previous = nullptr;
        node->next = head;

        if (head != nullptr) {
            head->previous = node;
        }

        head = node;

        if (tail == nullptr) {
            tail = node;
        }
    }

    // Removes a node from its current position.
    void removeNode(Node* node) {
        if (node == nullptr) {
            return;
        }

        if (node->previous != nullptr) {
            node->previous->next = node->next;
        } else {
            head = node->next;
        }

        if (node->next != nullptr) {
            node->next->previous = node->previous;
        } else {
            tail = node->previous;
        }

        node->previous = nullptr;
        node->next = nullptr;
    }

    // Moves an already existing node to the front.
    void moveToFront(Node* node) {
        if (node == head) {
            return;
        }

        removeNode(node);
        addToFront(node);
    }

public:
    LRUCache(int frameCapacity) {
        capacity = frameCapacity;
        currentSize = 0;
        head = nullptr;
        tail = nullptr;
    }

    /*
       Returns true when page hit occurs.
       Returns false when page fault occurs.

       evictedPage stores the page removed from memory.
       If no page is removed, it remains -1.
    */
    bool accessPage(int pageNumber, int& evictedPage) {
        evictedPage = -1;

        // Page is already present: Page Hit
        if (pageMap.find(pageNumber) != pageMap.end()) {
            Node* existingNode = pageMap[pageNumber];

            moveToFront(existingNode);

            return true;
        }

        // Page is not present: Page Fault

        // Empty frame is available
        if (currentSize < capacity) {
            Node* newNode = new Node(pageNumber);

            addToFront(newNode);
            pageMap[pageNumber] = newNode;

            currentSize++;
        }

        // All frames are full
        else {
            Node* leastRecentlyUsed = tail;

            evictedPage = leastRecentlyUsed->pageNumber;

            pageMap.erase(evictedPage);
            removeNode(leastRecentlyUsed);
            delete leastRecentlyUsed;

            Node* newNode = new Node(pageNumber);

            addToFront(newNode);
            pageMap[pageNumber] = newNode;
        }

        return false;
    }

    vector<int> getFrames() const {
        vector<int> frames;

        Node* current = head;

        while (current != nullptr) {
            frames.push_back(current->pageNumber);
            current = current->next;
        }

        return frames;
    }

    int getCapacity() const {
        return capacity;
    }

    ~LRUCache() {
        Node* current = head;

        while (current != nullptr) {
            Node* nextNode = current->next;
            delete current;
            current = nextNode;
        }

        pageMap.clear();
    }
};

/*==========================================================
                   DISPLAY HELPER FUNCTIONS
==========================================================*/

void printLine(char symbol = '=', int length = 70) {
    for (int i = 0; i < length; i++) {
        cout << symbol;
    }

    cout << '\n';
}

void printProjectHeader() {
    cout << '\n';
    printLine('=');
    cout << setw(49) << "VIRTUAL MEMORY MANAGER\n";
    cout << setw(52) << "PAGE REPLACEMENT SIMULATOR\n";
    printLine('=');

    cout << "DSA: Hash Map, Doubly Linked List, Vector, Searching and Traversal\n";
    printLine('-');
}

void displayReferenceString(const vector<int>& referenceString) {
    cout << "\nPage Reference String: ";

    for (int page : referenceString) {
        cout << page << " ";
    }

    cout << "\n";
}

void displayFrames(const vector<int>& frames, int frameCount) {
    cout << "[";

    for (int i = 0; i < frameCount; i++) {
        if (i < static_cast<int>(frames.size())) {
            cout << setw(3) << frames[i];
        } else {
            cout << setw(3) << "-";
        }

        if (i != frameCount - 1) {
            cout << " |";
        }
    }

    cout << " ]";
}

void displayTableHeader() {
    printLine('-', 82);

    cout << left
         << setw(8) << "Step"
         << setw(12) << "Page"
         << setw(15) << "Result"
         << setw(15) << "Evicted"
         << "Frames\n";

    printLine('-', 82);
}

void displayStep(
    int step,
    int page,
    const string& result,
    int evictedPage,
    const vector<int>& frames,
    int frameCount
) {
    cout << left
         << setw(8) << step
         << setw(12) << page
         << setw(15) << result;

    if (evictedPage == -1) {
        cout << setw(15) << "-";
    } else {
        cout << setw(15) << evictedPage;
    }

    displayFrames(frames, frameCount);

    cout << '\n';
}

void displayResult(
    const string& algorithmName,
    const SimulationResult& result
) {
    cout << '\n';
    printLine('=');

    cout << algorithmName << " SIMULATION RESULT\n";

    printLine('-');

    cout << "Total Page Requests : "
         << result.hits + result.faults << '\n';

    cout << "Page Hits           : "
         << result.hits << '\n';

    cout << "Page Faults         : "
         << result.faults << '\n';

    cout << fixed << setprecision(2);

    cout << "Hit Ratio           : "
         << result.getHitRatio() << "%\n";

    cout << "Fault Ratio         : "
         << result.getFaultRatio() << "%\n";

    printLine('=');
}

/*==========================================================
                      LRU SIMULATION
==========================================================*/

SimulationResult simulateLRU(
    const vector<int>& referenceString,
    int frameCount,
    bool showSteps
) {
    LRUCache cache(frameCount);
    SimulationResult result;

    if (showSteps) {
        cout << "\nLRU Page Replacement Simulation\n";
        cout << "Frame order: Most Recently Used -> Least Recently Used\n";

        displayTableHeader();
    }

    for (int index = 0;
         index < static_cast<int>(referenceString.size());
         index++) {

        int requestedPage = referenceString[index];
        int evictedPage = -1;

        bool isHit = cache.accessPage(
            requestedPage,
            evictedPage
        );

        if (isHit) {
            result.hits++;
        } else {
            result.faults++;
        }

        if (showSteps) {
            displayStep(
                index + 1,
                requestedPage,
                isHit ? "PAGE HIT" : "PAGE FAULT",
                evictedPage,
                cache.getFrames(),
                frameCount
            );
        }
    }

    if (showSteps) {
        printLine('-', 82);
        displayResult("LRU", result);
    }

    return result;
}

/*==========================================================
              OPTIMAL REPLACEMENT HELPER FUNCTION
==========================================================*/

int findOptimalVictim(
    const vector<int>& referenceString,
    const vector<int>& frames,
    int currentPosition
) {
    int farthestUse = -1;
    int victimIndex = 0;

    for (int frameIndex = 0;
         frameIndex < static_cast<int>(frames.size());
         frameIndex++) {

        int pageInFrame = frames[frameIndex];
        int nextUsePosition = -1;

        for (int futureIndex = currentPosition + 1;
             futureIndex < static_cast<int>(referenceString.size());
             futureIndex++) {

            if (referenceString[futureIndex] == pageInFrame) {
                nextUsePosition = futureIndex;
                break;
            }
        }

        /*
           If a page is never used again, it is the best
           page to remove.
        */
        if (nextUsePosition == -1) {
            return frameIndex;
        }

        /*
           Otherwise remove the page whose next use is
           farthest in the future.
        */
        if (nextUsePosition > farthestUse) {
            farthestUse = nextUsePosition;
            victimIndex = frameIndex;
        }
    }

    return victimIndex;
}

/*==========================================================
                    OPTIMAL SIMULATION
==========================================================*/

SimulationResult simulateOptimal(
    const vector<int>& referenceString,
    int frameCount,
    bool showSteps
) {
    vector<int> frames;
    SimulationResult result;

    if (showSteps) {
        cout << "\nOptimal Page Replacement Simulation\n";

        displayTableHeader();
    }

    for (int index = 0;
         index < static_cast<int>(referenceString.size());
         index++) {

        int requestedPage = referenceString[index];
        int evictedPage = -1;

        auto pagePosition = find(
            frames.begin(),
            frames.end(),
            requestedPage
        );

        bool isHit = pagePosition != frames.end();

        if (isHit) {
            result.hits++;
        } else {
            result.faults++;

            // Empty frame exists
            if (static_cast<int>(frames.size()) < frameCount) {
                frames.push_back(requestedPage);
            }

            // All frames are occupied
            else {
                int victimIndex = findOptimalVictim(
                    referenceString,
                    frames,
                    index
                );

                evictedPage = frames[victimIndex];
                frames[victimIndex] = requestedPage;
            }
        }

        if (showSteps) {
            displayStep(
                index + 1,
                requestedPage,
                isHit ? "PAGE HIT" : "PAGE FAULT",
                evictedPage,
                frames,
                frameCount
            );
        }
    }

    if (showSteps) {
        printLine('-', 82);
        displayResult("OPTIMAL", result);
    }

    return result;
}

/*==========================================================
                   COMPARISON FUNCTION
==========================================================*/

void compareAlgorithms(
    const vector<int>& referenceString,
    int frameCount
) {
    SimulationResult lruResult =
        simulateLRU(referenceString, frameCount, false);

    SimulationResult optimalResult =
        simulateOptimal(referenceString, frameCount, false);

    cout << '\n';
    printLine('=');
    cout << "LRU VS OPTIMAL COMPARISON\n";
    printLine('-');

    cout << left
         << setw(18) << "Algorithm"
         << setw(12) << "Hits"
         << setw(14) << "Faults"
         << setw(15) << "Hit Ratio"
         << "Fault Ratio\n";

    printLine('-', 70);

    cout << left
         << setw(18) << "LRU"
         << setw(12) << lruResult.hits
         << setw(14) << lruResult.faults
         << setw(14) << fixed << setprecision(2)
         << lruResult.getHitRatio()
         << lruResult.getFaultRatio() << "%\n";

    cout << left
         << setw(18) << "Optimal"
         << setw(12) << optimalResult.hits
         << setw(14) << optimalResult.faults
         << setw(14) << optimalResult.getHitRatio()
         << optimalResult.getFaultRatio() << "%\n";

    printLine('-');

    if (optimalResult.faults < lruResult.faults) {
        cout << "Optimal produced fewer page faults.\n";
    } else if (lruResult.faults < optimalResult.faults) {
        cout << "LRU produced fewer page faults for this sequence.\n";
    } else {
        cout << "Both algorithms produced the same number of page faults.\n";
    }

    printLine('=');
}

/*==========================================================
                    INPUT VALIDATION
==========================================================*/

int readPositiveInteger(const string& message) {
    int value;

    while (true) {
        cout << message;

        if (cin >> value && value > 0) {
            return value;
        }

        cout << "Invalid input. Enter a positive integer.\n";

        cin.clear();
        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n'
        );
    }
}

vector<int> readReferenceString(
    int numberOfReferences,
    int numberOfPages
) {
    vector<int> referenceString;

    cout << "\nValid page numbers are 0 to "
         << numberOfPages - 1 << ".\n";

    cout << "Enter page reference string:\n";

    for (int i = 0; i < numberOfReferences; i++) {
        int page;

        while (true) {
            cout << "Reference " << i + 1 << ": ";

            if (cin >> page &&
                page >= 0 &&
                page < numberOfPages) {

                referenceString.push_back(page);
                break;
            }

            cout << "Invalid page number. Enter a value from 0 to "
                 << numberOfPages - 1 << ".\n";

            cin.clear();
            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n'
            );
        }
    }

    return referenceString;
}

/*==========================================================
                         MAIN FUNCTION
==========================================================*/

int main() {
    printProjectHeader();

    int processSize =
        readPositiveInteger("Enter process size in KB: ");

    int pageSize =
        readPositiveInteger("Enter page size in KB: ");

    int numberOfPages =
        static_cast<int>(
            ceil(
                static_cast<double>(processSize) /
                pageSize
            )
        );

    int allocatedMemory = numberOfPages * pageSize;

    int internalFragmentation =
        allocatedMemory - processSize;

    cout << '\n';
    printLine('-');

    cout << "VIRTUAL MEMORY INFORMATION\n";

    printLine('-');

    cout << "Process Size           : "
         << processSize << " KB\n";

    cout << "Page Size              : "
         << pageSize << " KB\n";

    cout << "Number of Pages        : "
         << numberOfPages << '\n';

    cout << "Allocated Memory       : "
         << allocatedMemory << " KB\n";

    cout << "Internal Fragmentation : "
         << internalFragmentation << " KB\n";

    printLine('-');

    int frameCount =
        readPositiveInteger(
            "\nEnter number of physical frames: "
        );

    int numberOfReferences =
        readPositiveInteger(
            "Enter number of page references: "
        );

    vector<int> referenceString =
        readReferenceString(
            numberOfReferences,
            numberOfPages
        );

    int choice;

    do {
        cout << '\n';
        printLine('=');

        cout << "PAGE REPLACEMENT MENU\n";

        printLine('-');

        cout << "1. Run LRU Algorithm\n";
        cout << "2. Run Optimal Algorithm\n";
        cout << "3. Compare LRU and Optimal\n";
        cout << "4. Display Reference String\n";
        cout << "5. Exit\n";

        printLine('-');

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                displayReferenceString(referenceString);

                simulateLRU(
                    referenceString,
                    frameCount,
                    true
                );
                break;

            case 2:
                displayReferenceString(referenceString);

                simulateOptimal(
                    referenceString,
                    frameCount,
                    true
                );
                break;

            case 3:
                displayReferenceString(referenceString);

                compareAlgorithms(
                    referenceString,
                    frameCount
                );
                break;

            case 4:
                displayReferenceString(referenceString);
                break;

            case 5:
                cout << "\nSimulation closed successfully.\n";
                break;

            default:
                cout << "\nInvalid choice. Please try again.\n";
        }

    } while (choice != 5);

    return 0;
}