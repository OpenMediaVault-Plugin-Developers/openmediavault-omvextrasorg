/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @copyright Copyright (c) 2009-2014 Volker Theile
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/form/Panel.js")
// require("js/omv/workspace/window/Form.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/Model.js")
// require("js/omv/data/proxy/Rpc.js")
// require("js/omv/workspace/window/plugin/ConfigObject.js")
// require("js/omv/form/field/SharedFolderComboBox.js")

Ext.define("OMV.module.admin.privilege.sharedfolder.ResetPerms", {
    extend   : "OMV.workspace.form.Panel",
    requires : [
        "OMV.data.Store",
        "OMV.data.Model"
    ],

    rpcService   : "OmvExtrasOrg",
    rpcGetMethod : "getResetPerms",
    rpcSetMethod : "setResetPerms",
    plugins      : [{
        ptype : "configobject"
    }],

    getFormItems: function() {
        var me = this;
        return [{
            xtype      : "sharedfoldercombo",
            name       : "sharedfolderref",
            fieldLabel : _("Shared Folder")
        },{
            xtype       : "combo",
            name        : "mode",
            fieldLabel  : _("Permissions"),
            queryMode   : "local",
            store       : Ext.create("Ext.data.ArrayStore", {
                fields : [ "value", "text" ],
                data   : [
                    [ "700", _("Administrator: read/write, Users: no access, Others: no access") ],
                    [ "750", _("Administrator: read/write, Users: read-only, Others: no access") ],
                    [ "770", _("Administrator: read/write, Users: read/write, Others: no access") ],
                    [ "755", _("Administrator: read/write, Users: read-only, Others: read-only") ],
                    [ "775", _("Administrator: read/write, Users: read/write, Others: read-only") ],
                    [ "777", _("Everyone: read/write") ]
                ]
            }),
            displayField    : "text",
            valueField      : "value",
            allowBlank      : false,
            editable        : false,
            showItemTooltip : true,
            triggerAction   : "all",
            value           : "775",
            plugins         : [{
                ptype : "fieldinfo",
                text  : _("Reset folders and files to this file mode.")
            }]
        },{
                xtype      : "checkbox",
                name       : "clearacl",
                fieldLabel : _("Clear ACLs"),
                boxLabel   : _("Clear all ACLs."),
                checked    : false
        }];
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "resetperms",
    path      : "/privilege/sharedfolder",
    text      : _("Reset Permissions"),
    position  : 20,
    className : "OMV.module.admin.privilege.sharedfolder.ResetPerms"
});
