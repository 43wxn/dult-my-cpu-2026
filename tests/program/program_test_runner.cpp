#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct ProgramCase {
    std::string name;
    std::string source_path;
};

static std::vector<ProgramCase> load_manifest(const std::string& manifest_path) {
    std::vector<ProgramCase> cases;
    std::ifstream fin(manifest_path);
    if (!fin) {
        throw std::runtime_error("Cannot open manifest: " + manifest_path);
    }

    std::string name, path;
    while (fin >> name >> path) {
        cases.push_back({name, path});
    }
    return cases;
}

int main() {
    try {
        auto cases = load_manifest("../tests/program/program_test_manifest.txt");

        std::cout << "=== Program Test Runner Skeleton ===\n";
        std::cout << "Loaded " << cases.size() << " test case(s)\n\n";

        for (const auto& tc : cases) {
            std::cout << "[TODO] " << tc.name
                      << " -> " << tc.source_path << "\n";
        }

        std::cout << "\nCurrent status: skeleton ready.\n";
        std::cout << "Later modules will add compile/load/run/result-check.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 1;
    }
}