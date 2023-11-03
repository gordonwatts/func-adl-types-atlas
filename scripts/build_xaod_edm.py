from pathlib import Path
import argparse
import os

parser = argparse.ArgumentParser(description="Build XAOD EDM")
parser.add_argument("release", type=str, help="Release version")
parser.add_argument("output_file", type=str, help="Output file path")

args = parser.parse_args()

# For the docker container we need to know where this package sits so we can
# mount it.
package_path = Path(__file__).resolve().parent.parent

# For the output file, we need its name and the absolute path to where the
# user wants it written.
output_file = Path(args.output_file).resolve()
output_dir = output_file.parent
output_name = output_file.name

# next we build two bindings for the `output_dir` and `package_path`:
# 1. the path to the package inside the container
# 2. the path to the output file inside the container
# These are used in the `docker run` command below.
package_path_binding = f"{package_path}:/func_adl_xaod_types"
output_dir_binding = f"{output_dir}:/output"

# And the image name to build the container
image_name = f"gitlab-registry.cern.ch/atlas/athena/analysisbase:{args.release}"

# And the bash command to run inside the container
bash_cmd = (
    f'bash -c "/func_adl_xaod_types/scripts/build_run_incontainer.sh  {output_name}"'
)

# And build the docker command and run it.
docker_cmd = f"docker run --rm -v {package_path_binding} -v {output_dir_binding} {image_name} {bash_cmd}"
print(docker_cmd)
exit(os.system(docker_cmd))
