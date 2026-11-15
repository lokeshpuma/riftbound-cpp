#include <iostream>
#include <vector>
#include <string>

// Simple lightweight test harness
int g_totalTests = 0;
int g_passedTests = 0;

void RunPhysicsTests();
void RunCombatTests();
void RunProgressionTests();

int main() {
    std::cout << "===========================================\n";
    std::cout << "  Riftbound: Echoes of Ember - Unit Tests  \n";
    std::cout << "===========================================\n";

    RunPhysicsTests();
    RunCombatTests();
    RunProgressionTests();

    std::cout << "\n===========================================\n";
    std::cout << "Test Summary: " << g_passedTests << " / " << g_totalTests << " passed.\n";
    std::cout << "===========================================\n";

    return (g_passedTests == g_totalTests) ? 0 : 1;
}
