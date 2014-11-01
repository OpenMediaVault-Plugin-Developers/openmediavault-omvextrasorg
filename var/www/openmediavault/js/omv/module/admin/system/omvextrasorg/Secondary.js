/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2014 OpenMediaVault Plugin Developers
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

Ext.define("OMV.module.admin.system.omvextrasorg.Secondary", {
    extend : "OMV.workspace.form.Panel",

    rpcService   : "OmvExtrasOrg",
    rpcGetMethod : "getSecondary",
    rpcSetMethod : "setSecondary",

    initComponent : function() {
        var me = this;
        me.callParent(arguments);
        me.on("submit", function() {
            OMV.MessageBox.show({
                title      : _("Confirmation"),
                msg        : _("The information about available software is out-of-date. You need to reload the information about available software."),
                buttons    : Ext.MessageBox.OKCANCEL,
                buttonText : {
                    ok      : _("Reload"),
                    cancel  : _("Close")
                },
                fn : function(answer) {
                    if("cancel" === answer)
                        return;
                    OMV.RpcObserver.request({
                        title   : _("Downloading package information"),
                        msg     : _("The repository will be checked for new, removed or upgraded software packages."),
                        rpcData : {
                            service : "Apt",
                            method  : "update"
                        }
                    });
                },
                scope : me,
                icon  : Ext.Msg.QUESTION
            });
        }, me);
    },

    getFormItems: function() {
        return [{
            xtype         : "fieldset",
            title         : _("Additional Repositories"),
            fieldDefaults : {
                labelSeparator : ""
            },
            items         : [{
                border : false,
                html   : "<p>" + _("Only enable repositories you need.  Enabling unnecessary repositories can slow down your system.") + "</p>"
            },{
                xtype      : "checkbox",
                name       : "vbox",
                fieldLabel : _("Virtualbox"),
                boxLabel   : _("Enable Virtualbox repository."),
                checked    : true
            },{
                xtype      : "checkbox",
                name       : "plex",
                fieldLabel : _("Plex"),
                boxLabel   : _("Enable Plex Media Server repository"),
                checked    : false
            },{
                xtype      : "checkbox",
                name       : "greyhole",
                fieldLabel : _("Greyhole"),
                boxLabel   : _("Enable Greyhole repository"),
                checked    : false
            },{
                xtype      : "checkbox",
                name       : "vdr",
                fieldLabel : _("VDR"),
                boxLabel   : _("Enable e-tobi.net repository"),
                checked    : false
            },{
                xtype      : "checkbox",
                name       : "miller",
                fieldLabel : _("Download Managers"),
                boxLabel   : _("Enable download managers repository"),
                checked    : false,
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("Contains various download managers for newsgroups, torrents, usenet, and streaming")
                }]
            },{
                xtype      : "checkbox",
                name       : "millertesting",
                fieldLabel : _("Managers Testing"),
                boxLabel   : _("Enable download managers testing repository (release candidates)"),
                checked    : false
            },{
                xtype      : "checkbox",
                name       : "btsync",
                fieldLabel : _("BTSync"),
                boxLabel   : _("Enable BTSync repository"),
                checked    : false
            },{
                xtype      : "checkbox",
                name       : "vpn",
                fieldLabel : _("VPN"),
                boxLabel   : _("Enable VPN repository"),
                checked    : false,
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("Contains plugins for setting up virtual private networks")
                }]
            },{
                xtype      : "checkbox",
                name       : "zfs",
                fieldLabel : _("ZFS"),
                boxLabel   : _("Enable ZFS repository"),
                checked    : false,
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("Do not enable this repository unless you are using the ZFS filesystem (uncommon)")
                }]
            },{
                xtype      : "checkbox",
                name       : "gluster",
                fieldLabel : _("Gluster"),
                boxLabel   : _("Enable Gluster repository"),
                checked    : false,
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("Do not enable this repository unless you are using the GlusterFS filesystem (uncommon)")
                }]
            },{
                xtype      : "checkbox",
                name       : "mono",
                fieldLabel : _("Mono"),
                boxLabel   : _("Enable Mono and mono-project.com repositories"),
                checked    : false
            },{
                xtype      : "checkbox",
                name       : "hwraid",
                fieldLabel : _("HWRaid"),
                boxLabel   : _("Enable hwraid repository"),
                checked    : false
            }]
        }];
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "secondary",
    path      : "/system/omvextrasorg",
    text      : _("Secondary"),
    position  : 20,
    className : "OMV.module.admin.system.omvextrasorg.Secondary"
});
