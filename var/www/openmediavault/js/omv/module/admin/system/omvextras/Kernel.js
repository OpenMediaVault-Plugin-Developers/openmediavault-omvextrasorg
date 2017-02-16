/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2017 OpenMediaVault Plugin Developers
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
// require("js/omv/Rpc.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/Model.js")
// require("js/omv/data/proxy/Rpc.js")
// require("js/omv/window/MessageBox.js")

Ext.define("OMV.module.admin.system.omvextras.Kernel", {
    extend : "OMV.workspace.form.Panel",

    rpcService   : "OmvExtras",
    rpcGetMethod : "getKernel",

    hideTopToolbar: true,

    getFormItems : function() {
        var me = this;
        return [{
            xtype         : "fieldset",
            title         : _("Kernels"),
            fieldDefaults : {
                labelSeparator : ""
            },
            items         : [{
                name          : "kernels",
                xtype         : "combo",
                fieldLabel    : _("Installed Kernels"),
                allowBlank    : false,
                editable      : false,
                triggerAction : "all",
                displayField  : "name",
                valueField    : "key",
                store         : Ext.create("OMV.data.Store", {
                    autoLoad : true,
                    model    : OMV.data.Model.createImplicit({
                        idProperty : "key",
                        fields     : [
                            { name : "key", type : "integer" },
                            { name : "name", type : "string" }
                        ]
                    }),
                    proxy : {
                        type    : "rpc",
                        rpcData : {
                            service : "OmvExtras",
                            method  : "getKernelList"
                        }
                    }
                })
            },{
                xtype   : "button",
                name    : "setboot",
                text    : _("Set as default boot kernel"),
                scope   : this,
                handler : Ext.Function.bind(me.onSetBootButton, me, [ me ]),
                margin  : "5 0 8 0"
            },{
                xtype   : "button",
                name    : "backports",
                text    : _("Install Proxmox kernel"),
                scope   : this,
                handler : Ext.Function.bind(me.onCommandButton, me, [ "installpvekernel" ]),
                margin  : "5 0 8 10"
            },{
                border : false,
                html   : "<ul>" +
                           "<li>" + _("Setting the wrong default boot kernel may cause the system to be inaccessible.  The boot menu will still be available to select a different kernel.") + "</li>" +
                           "<li>" + _("Installing the Proxmox kernel will not uninstall other kernels.") + "</li>" +
                           "<li>" + _("Installing the Proxmox kernel on some systems may cause problems.") + "</li>" +
                           "<li>" + _("If the system does not boot using the Proxmox kernel, the boot menu will still have the option to boot other kernels.") + "</li>" +
                         "</ul>"
            }]
        },{
            xtype         : "fieldset",
            title         : _("Clonezilla"),
            fieldDefaults : {
                labelSeparator : ""
            },
            items         : [{
                xtype   : "button",
                name    : "installcz",
                text    : _("Install Clonezilla"),
                scope   : this,
                handler : Ext.Function.bind(me.onCommandButton, me, [ "installcz" ]),
                margin  : "5 0 0 10"
            },{
                border : false,
                html   : "<ul>" +
                             "<li>" + _("Downloads Clonezilla ISO and configures grub bootloader to allow booting from ISO.") + "</li>" +
                             "<li>" + _("SSH server is enabled by default.  Login with username: <b>user</b> and password: <b>live</b>") + "</li>" +
                             "<li>" + _("When connecting via ssh, the ssh key will be different than the OpenMediaVault ssh key and need to be updated on the client system.") + "</li>" +
                             "<li>" + _("IP Address will be set by DHCP.  Using static DHCP is recommended for headless servers.") + "</li>" +
                             "<li>" + _("When logging in remotely, start clonezilla with:  <b>sudo clonezilla</b>") + "</li>" +
                             "<li>" + _("ISO uses approximately 139 Mb in /boot directory on OS drive.") + "</li>" +
                         "</ul>"
            },{
                xtype   : "button",
                name    : "rebootcz",
                text    : _("Reboot to Clonezilla Once"),
                scope   : this,
                handler : Ext.Function.bind(me.onCommandButton, me, [ "rebootcz" ]),
                margin  : "0 0 0 10"
            },{
                border : false,
                html   : "<ul><li>" + _("Sets grub bootloader to boot from Clonezilla ISO <b>ONE</b> time.") + "</li></ul>"
            }]
        },{
            xtype         : "fieldset",
            title         : _("GParted Live"),
            fieldDefaults : {
                labelSeparator : ""
            },
            items         : [{
                xtype   : "button",
                name    : "installgp",
                text    : _("Install GParted Live"),
                scope   : this,
                handler : Ext.Function.bind(me.onCommandButton, me, [ "installgp" ]),
                margin  : "5 0 0 10"
            },{
                border : false,
                html   : "<ul>" +
                             "<li>" + _("Downloads GParted Live ISO and configures grub bootloader to allow booting from ISO.") + "</li>" +
                             "<li>" + _("Not recommended for headless servers.  SSH is not enabled by default.") + "</li>" +
                             "<li>" + _("Default username: <b>user</b> and password: <b>live</b>") + "</li>" +
                             "<li>" + _("IP Address will be set by DHCP.") + "</li>" +
                             "<li>" + _("ISO uses approximately 219 Mb in /boot directory on OS drive.") + "</li>" +
                         "</ul>"
            },{
                xtype   : "button",
                name    : "rebootgp",
                text    : _("Reboot to GParted Live Once"),
                scope   : this,
                handler : Ext.Function.bind(me.onCommandButton, me, [ "rebootgp" ]),
                margin  : "0 0 0 10"
            },{
                border : false,
                html   : "<ul><li>" + _("Sets grub bootloader to boot from GParted Live ISO <b>ONE</b> time.") + "</li></ul>"
            }]
        },{
            xtype         : "fieldset",
            title         : _("SystemRescueCD"),
            fieldDefaults : {
                labelSeparator : ""
            },
            items         : [{
                xtype   : "button",
                name    : "installsys",
                text    : _("Install SystemRescueCD"),
                scope   : this,
                handler : Ext.Function.bind(me.onCommandButton, me, [ "installsys" ]),
                margin  : "5 0 0 10"
            },{
                border : false,
                html   : "<ul>" +
                             "<li>" + _("Downloads SystemRescuecd ISO and configures grub bootloader to allow booting from ISO.") + "</li>" +
                             "<li>" + _("SSH server is enabled by default.  Login with username: <b>root</b> and password: <b>openmediavault</b>") + "</li>" +
                             "<li>" + _("When connecting via ssh, the ssh key will be different than the OpenMediaVault ssh key and need to be updated on the client system.") + "</li>" +
                             "<li>" + _("IP Address will be set by DHCP.  Using static DHCP is recommended for headless servers.") + "</li>" +
                             "<li>" + _("ISO uses approximately 381 Mb in /boot directory on OS drive.") + "</li>" +
                         "</ul>"
            },{
                xtype   : "button",
                name    : "rebootsys",
                text    : _("Reboot to SystemRescueCD Once"),
                scope   : this,
                handler : Ext.Function.bind(me.onCommandButton, me, [ "rebootsys" ]),
                margin  : "0 0 0 10"
            },{
                border : false,
                html   : "<ul><li>" + _("Sets grub bootloader to boot from Clonezilla ISO <b>ONE</b> time.") + "</li></ul>"
            }]
        }];
    },

    onCommandButton : function(cmd) {
        var me = this;
        switch(cmd) {
            case "installpvekernel":
                title = _("Install Proxmox kernel ...");
                break;
            case "installcz":
                title = _("Install Clonezilla ISO ...");
                break;
            case "installgp":
                title = _("Install GParted Live ISO ...");
                break;
            case "installsys":
                title = _("Install SystemRescueCD ISO ...");
                break;
            case "rebootcz":
                title = _("Reboot to Clonezilla ...");
                break;
            case "rebootgp":
                title = _("Reboot to GParted Live ...");
                break;
            case "rebootsys":
                title = _("Reboot to SystemRescueCD ...");
                break;
            default:
                title = _("Cleaning Apt Files and Lists...");
                cmd = "aptclean";
        }
        Ext.create("OMV.window.Execute", {
            title          : title,
            rpcService     : "OmvExtras",
            rpcMethod      : "doCommand",
            rpcParams      : {
                "command" : cmd
            },
            hideStopButton : true,
            listeners      : {
                scope     : me,
                exception : function(wnd, error) {
                    OMV.MessageBox.error(null, error);
                }
            }
        }).show();
    },

    onSetBootButton : function() {
        var me = this;
        var key = me.findField("kernels").getValue();
        OMV.MessageBox.wait(null, _("Setting boot kernel ..."));
        OMV.Rpc.request({
            scope       : me,
            relayErrors : false,
            rpcData     : {
                service  : "OmvExtras",
                method   : "setBootKernel",
                params   : {
                    key : key
                }
            },
            success : function(id, success, response) {
                me.doReload();
                OMV.MessageBox.hide();
            }
        });
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "kernel",
    path      : "/system/omvextras",
    text      : _("Kernel"),
    position  : 20,
    className : "OMV.module.admin.system.omvextras.Kernel"
});
