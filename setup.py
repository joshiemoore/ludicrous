from setuptools import setup, Extension


def main():
    setup(
        name="ludicrous",
        version="0.0.0",
        description="ludicrous speed JSON API framework",
        author="Joshua Moore",
        author_email="jxm5210@g.rit.edu",
        license="GPLv2",
        url="https://github.com/joshiemoore/ludicrous/",
        ext_modules=[
            Extension(
                "ludicrous",
                [
                    "src/ludicrous.c",
                    "src/server/server.c",
                    "src/json/json_encode.c",
                    "lib/mongoose/mongoose.c"
                ],
                include_dirs=[
                    "include",
                    "lib/mongoose"
                ]
            )
        ],
    )

if __name__ == "__main__":
    main()
