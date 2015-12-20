------------------------------------------------------------------------------------------------------------------------
-- Search For Node
------------------------------------------------------------------------------------------------------------------------
local treeView
local searchStringTB

table.append = function(orig, src)
  for i,v in pairs(src) do
    table.insert(orig,v)
  end
end

local function getAncestry(v)
  local retval = {}
  local path,node = splitNodePath(v)
  if (path~="") then
    retval = getAncestry(path)
  end
  table.append(retval,{node})
  return retval
end

------------------------------------------------------------------------------------------------------------------------
local function navigateToSelected()
  -- build up a path of the selected Item
  local treeItem = treeView:getSelectedItem()
  local path = treeItem:getValue()
  treeItem = treeItem:getParent()
  
  while treeItem:getValue()~="Root" do
    path = treeItem:getValue() .. "|" .. path
    treeItem = treeItem:getParent()
  end
  
  if objectExists(path) then
    parent,_ = splitNodePath(path)
    setCurrentGraph(parent)
    select(path)
    
  end
end


------------------------------------------------------------------------------------------------------------------------
local function addNodeAndAncestors(v)
  -- recursively add parents until we have correct tree structure
  local path, node = splitNodePath(v)
  local treeItem
  if (path ~= "") then
    treeItem = addNodeAndAncestors(path)
  else
    treeItem = treeView:getRoot()
  end

  -- tree item is now a valid tree item that contains all our parents
  -- make sure we haven't already got this child
  local retval
  for i = 1, treeItem:getNumChildren() do
    if treeItem:getChild(i):getValue() == node then
      -- already have this one
      retval = treeItem:getChild(i)
      break
    end
  end
  if (retval == nil) then
    -- Haven't found it
    retval = treeItem:addChild(node)
    retval:setUserDataString(v)
  end
  return retval
end

local function onSearchStringChanged()
  local root = treeView:getRoot()
  root:clearChildren()
  
  -- filter contents of treeview based on the search string.
  local search = searchStringTB:getValue()

  -- Build a table of nodes that we're interested in
  local t = {}
  table.append(t, ls("BlendTree"))
  table.append(t, ls("StateMachine"))
  table.append(t, ls("State"))
  for i,v in pairs(listTypes("BlendNode")) do
    table.append(t, ls(v))
  end
  for i,v in pairs(listTypes("FilterNode")) do
    table.append(t, ls(v))
  end
  for i,v in pairs(listTypes("Transition")) do
    table.append(t, ls(v))
  end

  for i,v in pairs(t) do
    local _,node = splitNodePath(v)    
    if (search == "" or string.find(node:upper(),search:upper()) ~= nil) then
      addNodeAndAncestors(v)
    end
  end

end

------------------------------------------------------------------------------------------------------------------------
local selectionChanging = false

local function treeViewSelChanged()
  if selectionChanging then return end
  if treeView == nil then return end
  selectionChanging = true
  local sel = treeView:getSelectedItems()

  local paths = {}
  for _,item in pairs(sel) do
    table.append(paths,{item:getUserDataString()})
  end
  
  -- do the selection
  select(paths)
  
  selectionChanging = false
end

local function mcSelChanged()
  if selectionChanging then return end
  if treeView == nil then return end
  selectionChanging = true
  treeView:clearSelection()

  local visibleItem = nil
  -- loop over the selection
  for _,v in pairs(ls("Selection")) do
    local treeItem = treeView:getRoot()
    -- make sure ancestors are visible
    local ancestors = getAncestry(v)
    for i,u in pairs(ancestors) do
      local foundChild = nil
      -- search for the correctly named child
      for j = 1, treeItem:getNumChildren() do
        if treeItem:getChild(j):getValue() == u then
          foundChild = treeItem:getChild(j)
          break
        end
      end

      if(foundChild==nil) then
        -- this item was not found - continue with next item in selection
        break
      else
        if (i == table.getn(ancestors)) then
          -- Last in lst, so select it
          treeView:selectItem(foundChild)
          visibleItem = foundChild
        end
      end
      
      -- Recurse into the next item
      treeItem = foundChild
    end
  end
  
  if (visibleItem ~= nil) then 
    -- ensure the (last found) tree item is visible
    treeView:ensureItemVisible(visibleItem)
  end  
  
    -- select the corresponding in the tree view
  selectionChanging = false
end


------------------------------------------------------------------------------------------------------------------------
function addOverviewWindow(layoutManager)

  dlg = layoutManager:addPanel{name = "Overview", caption = "Overview", resize = true, size = { width = 300, height = 300 }}

  dlg:beginVSizer()
    treeView = dlg:addTreeControl{
      name = "TreeView",
      flags = "expand;hideRoot;multiSelect",
      proportion = 1,
      onItemActivated = navigateToSelected,
      onSelectionChanged = treeViewSelChanged }
    dlg:beginHSizer{flags = "expand"}
      dlg:addStaticText{text = "Search Filter:"}
      searchStringTB = dlg:addTextBox{name = "SearchString", flags = "expand", proportion=1,  onChanged = onSearchStringChanged}
      dlg:addButton{label = "X", onClick = function() 
                                             searchStringTB:setValue("")
                                             onSearchStringChanged()
                                           end}
    dlg:endSizer()
  dlg:endSizer()
  
  searchStringTB:setValue("")
  onSearchStringChanged()
end

registerEventHandler("mcSelectionChange", mcSelChanged)
-- TODO: Add a column for current visible graph

registerEventHandler("mcNodeCreated", onSearchStringChanged)
registerEventHandler("mcNodeDestroyed", onSearchStringChanged)
registerEventHandler("mcNodeRenamed", onSearchStringChanged)

registerEventHandler("mcEdgeCreated", onSearchStringChanged)
registerEventHandler("mcEdgeDestroyed", onSearchStringChanged)
registerEventHandler("mcEdgeRenamed", onSearchStringChanged)

registerEventHandler("mcControlParameterCreated", onSearchStringChanged)
registerEventHandler("mcControlParameterDestroyed", onSearchStringChanged)
registerEventHandler("mcControlParameterRenamed", onSearchStringChanged)
