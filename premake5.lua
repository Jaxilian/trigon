local Workspace = require("workspace")
local Project   = require("project")
local Vulkan    = require("vulkan")


local projects  = {
    "test",
    "trigon"
}

local cpp_enabled   = true
local show_console  = false

Workspace.New(name)

Project.Begin(name, cpp_enabled, Project.TYPES.APP, show_console)
Vulkan.Link()
Project.End(name)