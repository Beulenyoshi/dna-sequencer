
import os

SRC_DIR = "src/"

cpp_extensions = (".cpp", ".cxx", ".c++", ".cc", ".cp", ".c", ".i", ".ii", ".h", ".h++", ".hpp", ".hxx", ".hh", ".inl", ".inc", ".ipp", ".ixx", ".txx", ".tpp", ".tcc", ".tpl")
for root, dir, files in os.walk(SRC_DIR):
    for file in files:
        if file.endswith(cpp_extensions):
            os.system("clang-format -i " + root + "/" + file)
