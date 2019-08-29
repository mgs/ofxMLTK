class Taglib < Formula
  desc "Audio metadata library"
  homepage "https://taglib.github.io/"
  url "https://taglib.github.io/releases/taglib-1.11.1.tar.gz"
  sha256 "b6d1a5a610aae6ff39d93de5efd0fdc787aa9e9dc1e7026fa4c961b26563526b"
  head "https://github.com/taglib/taglib.git"

  depends_on "cmake" => :build

  def install
    system "cmake", "-DWITH_MP4=ON", "-DWITH_ASF=ON", "-DBUILD_SHARED_LIBS=OFF",
                    *std_cmake_args
    system "make", "install"
  end

  test do
    assert_match version.to_s, shell_output("#{bin}/taglib-config --version")
  end
end
