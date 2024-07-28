local Workspace = require("Workspace")
local Project   = require("Project")
local Vulkan    = require("Vulkan")

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
Project.UseModule(trigon, "tgstd")
Project.UseModule(trigon, "tgcore")
Project.UseModule(trigon, "trigon")
Project.End(trigon)

-- BEGIN APP

Project.Begin(
    app_name,
    cpp_enabled, 
    Project.TYPES.APP, 
    show_console
)

Project.UseModule(app_name, "test")
Project.Link(trigon)
Project.End(app_name)



