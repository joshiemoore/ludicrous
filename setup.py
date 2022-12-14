from setuptools import setup, Extension, find_packages


def main():
    setup(
        name="ludicrous",
        version="0.3.0",
        description="ludicrous speed JSON API framework for Python",
        author="Joshua Moore",
        author_email="jxm5210@g.rit.edu",
        license="GPLv2",
        url="https://github.com/joshiemoore/ludicrous/",
        packages=["ludicrous"],
        ext_modules=[
            Extension(
                "ludicrous.server",
                [
                    "src/ludicrous.c",
                    "src/server/server.c",
                    "src/server/urlnode.c",
                    "src/json/json_encode.c",
                    "src/json/json_decode.c",
                    "lib/mongoose/mongoose.c"
                ],
                include_dirs=[
                    "include",
                    "lib/mongoose",
                    "lib/jsmn"
                ]
            )
        ],
    )

if __name__ == "__main__":
    main()
