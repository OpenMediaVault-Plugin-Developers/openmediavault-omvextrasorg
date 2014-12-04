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
// require("js/omv/toolbar/Tip.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/Model.js")
// require("js/omv/data/proxy/Rpc.js")
// require("js/omv/workspace/window/plugin/ConfigObject.js")
// require("js/omv/form/field/SharedFolderComboBox.js")

Ext.define("OMV.module.admin.privilege.sharedfolder.ResetPerms", {
    extend   : "OMV.workspace.form.Panel",
    requires : [
        "OMV.workspace.window.plugin.ConfigObject"
    ],
    uses     : [
        "OMV.data.Model",
        "OMV.data.Store"
    ],

    rpcService   : "OmvExtrasOrg",
    rpcGetMethod : "getResetPerms",
    rpcSetMethod : "setResetPerms",

    hideResetButton : true,

    getButtonItems : function() {
        var me = this;
        var items = me.callParent(arguments);
        items.push({
            id       : me.getId() + "-resetperms",
            xtype    : "button",
            text     : _("Reset Permissions"),
            icon     : "images/refresh.png",
            iconCls  : Ext.baseCSSPrefix + "btn-icon-16x16",
            scope    : me,
            handler  : Ext.Function.bind(me.onResetPermsButton, me, [ me ])
        });
        return items;
    },

    getFormItems : function() {
        var me = this;
        return [{
            xtype         : "fieldset",
            title         : _("Reset Permissions"),
            fieldDefaults : {
                labelSeparator : ""
            },
            items         : [{
                xtype      : "sharedfoldercombo",
                name       : "sharedfolderref",
                fieldLabel : _("Shared Folder"),
                allowNone  : true,
                allowBlank : true
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
            }]
        }];
    },

    onResetPermsButton: function() {
        var me = this;
        me.doSubmit();
        var folder = me.findField("sharedfolderref").getValue();
        if (folder != "") {
            var wnd = Ext.create("OMV.window.Execute", {
                title      : _("Reset permissions ..."),
                rpcService : "OmvExtrasOrg",
                rpcMethod  : "doResetPerms",
                hideStartButton : true,
                hideStopButton  : true,
                listeners       : {
                    scope     : me,
                    finish    : function(wnd, response) {
                        wnd.appendValue(_("Done..."));
                        wnd.setButtonDisabled("close", false);
                    },
                    exception : function(wnd, error) {
                        OMV.MessageBox.error(null, error);
                        wnd.setButtonDisabled("close", false);
                    }
                }
            });
            wnd.setButtonDisabled("close", true);
            wnd.show();
            wnd.start();
        }
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "resetperms",
    path      : "/privilege/sharedfolder",
    text      : _("Reset Permissions"),
    position  : 20,
    className : "OMV.module.admin.privilege.sharedfolder.ResetPerms"
});
