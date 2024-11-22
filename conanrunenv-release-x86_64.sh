script_folder="/home/ubuntu/openmind"
echo "echo Restoring environment" > "$script_folder/deactivate_conanrunenv-release-x86_64.sh"
for v in LD_LIBRARY_PATH DYLD_LIBRARY_PATH
do
    is_defined="true"
    value=$(printenv $v) || is_defined="" || true
    if [ -n "$value" ] || [ -n "$is_defined" ]
    then
        echo export "$v='$value'" >> "$script_folder/deactivate_conanrunenv-release-x86_64.sh"
    else
        echo unset $v >> "$script_folder/deactivate_conanrunenv-release-x86_64.sh"
    fi
done


export LD_LIBRARY_PATH="/home/ubuntu/.conan2/p/hired21d77b628160a/p/lib:$LD_LIBRARY_PATH"
export DYLD_LIBRARY_PATH="/home/ubuntu/.conan2/p/hired21d77b628160a/p/lib:$DYLD_LIBRARY_PATH"