import subprocess
import platform

if platform.system() == "windows":
    subprocess.run(["glslc", "test.vert -o ../spv/test_vert.spv"]) 
    subprocess.run(["glslc", "test.frag -o ../spv/test_frag.spv"]) 
