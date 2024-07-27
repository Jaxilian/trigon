local Workspace = require("workspace")
local Project   = require("project")
local Vulkan    = require("vulkan")
local Paths     = require("paths")

Paths.UseTrigonStandard() 

-- To use custom path, uncomment below lines:
-- Paths.PATH_TO_INCLUDES   = "MY/PATH/TO/INCLUDES"
-- Paths.PATH_TO_LIBRARIES  = "MY/PATH/TO/LIBRARIES"
-- Paths.PATH_TO_OUTPUT     = "MY/PATH/TO/OUTPUT"

local cpp_enabled   = false
local show_console  = true

local trigon = "trigon"
local app_name = "test"
local start_project = app_name

Workspace.New(start_project)

-- BEGIN TRIGON 

Project.Begin(
    trigon,
    cpp_enabled,
    Project.TYPES.LIB
)

Vulkan.Link()

Project.End(trigon)

-- BEGIN APP

Project.Begin(
    app_name,
    cpp_enabled, 
    Project.TYPES.APP, 
    show_console
)

Project.Link(trigon)

Project.End(app_name)



