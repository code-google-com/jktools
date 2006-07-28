project.name = "jktools"

package = newpackage()
package.name = "jklview"
package.language = "c++"
package.kind = "exe"
package.config["Debug"].target = "bin/Debug/jklview"
package.config["Release"].target = "bin/Release/jklview"
package.config["Debug"].libpaths = { "lib/Debug" }
package.config["Release"].libpaths = { "lib/Release" }

package.config["Debug"].defines =
{
	"DEBUG" 
}

package.files =
{
	"jklview.cpp"
}

package.links =
{
	"jkllib"
}

package = newpackage()
package.name = "jkl2map"
package.language = "c++"
package.kind = "exe"
package.config["Debug"].target = "bin/Debug/jkl2map"
package.config["Release"].target = "bin/Release/jkl2map"
package.config["Debug"].libpaths = { "lib/Debug" }
package.config["Release"].libpaths = { "lib/Release" }

package.config["Debug"].defines =
{
	"DEBUG" 
}

package.files =
{
	"jkl2map.cpp"
}

package.links =
{
	"jkllib"
}

package = newpackage()
package.name = "ungob"
package.language = "c++"
package.kind = "exe"
package.config["Debug"].target = "bin/Debug/ungob"
package.config["Release"].target = "bin/Release/ungob"
package.config["Debug"].libpaths = { "lib/Debug" }
package.config["Release"].libpaths = { "lib/Release" }

package.config["Debug"].defines =
{
	"DEBUG" 
}

package.files =
{
	"gob.h",
	"ungob.cpp"
}

package = newpackage()
package.name = "jkllib"
package.language = "c++"
package.kind = "lib"
package.config["Debug"].target = "lib/Debug/jkllib"
package.config["Release"].target = "lib/Release/jkllib"

package.config["Debug"].defines =
{
	"DEBUG" 
}

package.files =
{
	"jkllib.cpp",
	"jkllib.h"
}

function doclean(cmd, arg)
	docommand(cmd, arg)
	os.rmdir("obj")
	os.rmdir("lib")
	os.rmdir("bin")
end
