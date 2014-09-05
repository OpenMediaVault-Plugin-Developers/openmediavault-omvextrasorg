/**
 * @license     http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author      Volker Theile <volker.theile@openmediavault.org>
 * @author      OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright   Copyright (c) 2009-2013 Volker Theile
 * @copyright   Copyright (c) 2013-2014 OpenMediaVault Plugin Developers
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
// require("js/omv/workspace/panel/Textarea.js")

/**
 * @class OMV.module.admin.diagnostic.system.SupportInfo
 * @derived OMV.workspace.panel.Textarea
 */
Ext.define("OMV.module.admin.diagnostic.system.SupportInfo", {
    extend   : "OMV.workspace.panel.Textarea",
    requires : [
        "OMV.Rpc",
        "OMV.window.MessageBox"
    ],

    info : "style1",

    hideDownloadButton : false,

    rpcService : "OmvExtrasOrg",
    rpcMethod  : "getSupportInfo",

    getTopToolbarItems: function() {
        var me = this;
        var items = me.callParent(arguments);

        items.push({
            id       : me.getId() + "-send",
            xtype    : "button",
            text     : _("Send"),
            icon     : "images/mail.png",
            iconCls  : Ext.baseCSSPrefix + "btn-icon-16x16",
            handler  : Ext.Function.bind(me.onSendButton, me, [ me ]),
            scope    : me
        },{
            id            : me.getId() + "-info",
            xtype         : "combo",
            queryMode     : "local",
            store         : [
                [ "style1", "Style #1" ],
                [ "style2", "Style #2" ]
            ],
            allowBlank    : false,
            editable      : false,
            triggerAction : "all",
            value         : me.info,
            listeners     : {
                scope  : me,
                change : function(combo, value) {
                    this.info = value;
                    this.rpcParams.info = value;
                    me.showSupportInfo();
                }
            }
        });
        return items;
    },

    showSupportInfo : function() {
        var me = this;
        OMV.Rpc.request({
            scope    : this,
            callback : function(id, success, response) {
                this.setValue(response);
            },
            relayErrors : false,
            rpcData     : {
                service  : "OmvExtrasOrg",
                method   : "getSupportInfo",
                params   : {
                    info  : me.info
                }
            }
        });
    },

    onSendButton: function() {
        var me = this;
        Ext.create("OMV.window.Execute", {
            title      : _("Send Support Info"),
            rpcService : "OmvExtrasOrg",
            rpcMethod  : "doSend",
            listeners  : {
                scope     : me,
                exception : function(wnd, error) {
                    OMV.MessageBox.error(null, error);
                }
            }
        }).show();
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "supportinfo",
    path      : "/diagnostic/system",
    text      : _("Support Info"),
    position  : 26,
    className : "OMV.module.admin.diagnostic.system.SupportInfo"
});
