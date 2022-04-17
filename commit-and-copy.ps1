Try
{
    Write-Host -ForeGroundColor Blue "Processing commit ...";

    # TeknoParrot install path
    $TeknoPath = "D:\Games\TeknoParrot";

    # Path to OpenParrot x86 Files
    $OpenParrot32 = Join-Path -Path $TeknoPath -ChildPath "OpenParrotWin32";

    # 32-bit OpenParrot files
    $OpenParrot32Files = @(
        "iDmacDrv32.dll", 
        "OpenParrot.dll",
        "OpenParrotKonamiLoader.exe",
        "OpenParrotLoader.exe"
    );

    # Path to OpenParrot x64 files
    $OpenParrot64 = Join-Path -Path $TeknoPath -ChildPath "OpenParrotx64";

    # 64-Bit OpenParrot files
    $OpenParrot64Files = @(
        "iDmacDrv64.dll",
        "OpenParrot64.dll",
        "OpenParrotLoader64.exe"
    );

    # Get the user's current location
    $Location = Get-Location;

    # Set the location to the script path
    Set-Location $PSScriptRoot;

    # Get the build output path
    $BuildPath = Join-Path -Path $PSScriptRoot -ChildPath "build\bin\release\output";

    # Add all files to the repo
    git add .;

    # Ensure the test branch is checked out
    git checkout test

    # Commit the changes to the repo
    git commit -m "[$(Get-Date)] pre-test commit";

    # Push the changes to the test branch
    git push origin test

    # Master branch will need to be merged manually once test is ensured working
    
    # Copy the win32 files from the build path to the tekno path
    ForEach($File in $OpenParrot32Files)
    {
        # Get the path for the file we are copying
        $Path = Join-Path -Path $BuildPath -ChildPath $File;

        # Copy the file to the win32 folder, overwriting existing
        Copy-Item -Path $Path -Destination $OpenParrot32 -Force;
    }

    # Copy the win64 files from the build path to the tekno path
    ForEach($File in $OpenParrot64Files)
    {
        # Get the path for the file we are copying
        $Path = Join-Path -Path $BuildPath -ChildPath $File;

        # Copy the file to the win64 folder, overwriting existing
        Copy-Item -Path $Path -Destination $OpenParrot64 -Force;
    }

    Write-Host -ForeGroundColor Green "Commit processed and copied successfully.";
}
Catch # Failed to commit / copy files
{
    Write-Host -ForeGroundColor Red "Failed to process commit! Reason: $($_.Exception.Message)";
}