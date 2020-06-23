#include <gtest/gtest.h>
#include "../passthrough.h"
#include "../sea.h"
#include "../config.h"
#include <stdlib.h>

// need to add symlink tests here
TEST(Passthrough, GetCanonical) {

    initialize_passthrough_if_necessary();

    const char paths[6][PATH_MAX] = { "mount", "./mount/subdir", "../mount/", "/dev/shm/tmp", "/", NULL };

    for(auto path: paths){
        char* actualpath;
        char* expected = (char*) malloc(sizeof(char*) * PATH_MAX);
    
        actualpath = make_file_name_canonical(path);
        realpath(path, expected);
        
        if(path == NULL || path[0] == '\0') {
            if (actualpath == expected) {
                ASSERT_TRUE(true);
            }
        }
        else
            ASSERT_STREQ(actualpath, expected);

        free(expected);
    }

}

TEST(Passthrough, GetPassCanonical) {
    char path[PATH_MAX];
    char passpath[PATH_MAX];

    char * mount_dir = (char*) malloc(sizeof(char*)*PATH_MAX);
    char * source_0 = (char*) malloc(sizeof(char*)*PATH_MAX);
    char * source_1 = (char*) malloc(sizeof(char*)*PATH_MAX);
    
    realpath("../mount", mount_dir);
    realpath("../source", source_0);
    realpath("/dev/shm/source", source_1);

    get_pass_canonical(path, passpath, mount_dir, source_0, 0);

    ASSERT_STREQ(passpath, source_0);
    ASSERT_STREQ(path, mount_dir);    

    get_pass_canonical(path, passpath, mount_dir, source_1, 0);

    ASSERT_STREQ(passpath, source_1);
    ASSERT_STREQ(path, mount_dir);    

    get_pass_canonical(path, passpath, mount_dir, source_0, 1);
    ASSERT_STREQ(passpath, mount_dir);
    ASSERT_STREQ(path, source_0);    
}

TEST(Passthrough, CheckIfSeaPath) {

    char oldpaths[4][PATH_MAX] = { "mount/test.txt", "mount/subdir/test.txt", "/dev/shm/test.txt", "/" };
    char expaths[4][PATH_MAX] = { "source/test.txt", "source/subdir/test.txt", "/dev/shm/test.txt", "/" };
    char path[PATH_MAX];

    realpath("mount", path);

    int i = 0;
    for (auto op: oldpaths) {
        // need to reinitialize each time otherwise may retain tail of previous value
        char passpath[PATH_MAX];
        realpath("source", passpath);

        char* actualpath;
        char expected[PATH_MAX];
        actualpath = make_file_name_canonical(op);
        realpath(expaths[i], expected);
        int res = check_if_seapath(path, actualpath, passpath);
       
        if (i < 2)
            ASSERT_EQ(res, 1);
        else
            ASSERT_EQ(res, 0);

        ASSERT_STREQ(passpath, expected);
        ++i;
    }
}

TEST(Passthrough, GetRealPath) {
    initialize_passthrough_if_necessary();

    // test relative mount path
    const char* oldpath = (const char*) "mount/file.txt"; 
    char passpath[PATH_MAX];
    pass_getpath(oldpath, passpath, 0);
    char rp[PATH_MAX];
    realpath("source/file.txt", rp);
    ASSERT_STREQ(passpath, rp);

    // test masking
    const char* oldpath_1 = (const char*) "source/file.txt"; 
    pass_getpath(oldpath_1, passpath, 1);
    realpath("mount/file.txt", rp);
    ASSERT_STREQ(passpath, rp);

    // test non mount path without masking
    const char* oldpath_2 = (const char*) "/dev/shm/file.txt"; 
    pass_getpath(oldpath_2, passpath, 0);
    strcpy(rp, oldpath_2);
    ASSERT_STREQ(passpath, rp);

    // test non mount path with masking
    pass_getpath(oldpath_2, passpath, 1);
    strcpy(rp, oldpath_2);
    ASSERT_STREQ(passpath, rp);

}


TEST(Sea, InitializeSea) {

    initialize_sea();
    // 2 mounts * 2 ('.' & '..') + 2 subdirs * 2 mounts * 3 (dirname & '.' & '..') + 3 files
    ASSERT_EQ(sea_files.size(), 19);

    initialize_passthrough_if_necessary();
    config sea_config = get_sea_config();

    char *added_dir = new char[PATH_MAX];
    strcpy(added_dir, sea_config.source_mounts[1]);
    strcat(added_dir, "/newdir");

    ((funcptr_mkdir)libc_mkdir)(added_dir, 0666);
    sea_files.clear();
    initialize_sea();
    // 19 from before + 6 ((dirname, ., ..)*2)
    ASSERT_EQ(sea_files.size(), 25);


}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
