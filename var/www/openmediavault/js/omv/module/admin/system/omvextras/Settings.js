/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2019 OpenMediaVault Plugin Developers
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
// require("js/omv/form/plugin/LinkedFields.js")
// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/form/Panel.js")
// require("js/omv/workspace/window/Form.js")
// require("js/omv/workspace/window/plugin/ConfigObject.js")

Ext.define("OMV.module.admin.system.omvextras.Repos", {
    extend: "OMV.workspace.form.Panel",

    rpcService: "OmvExtras",
    rpcGetMethod: "getSettings",
    rpcSetMethod: "setSettings",

    initComponent: function() {
        var me = this;
        OMV.Rpc.request({
            scope: this,
            callback: function(id, success, response) {
                var parent = me.up('tabpanel');

                if (!parent)
                    return;

                var arch = response.arch;
                var kernelPanel = parent.down('panel[title=' + _("Kernel") + ']');

                if (kernelPanel) {
                    var n = arch.indexOf("arm");
                    if (n < 0) {
                        kernelPanel.tab.show();
                    } else {
                        kernelPanel.tab.hide();
                    }
                }
            },
            relayErrors: false,
            rpcData: {
                service: "OmvExtras",
                method: "getArch"
            }
        });
        me.callParent(arguments);
    },

    getButtonItems: function() {
        var me = this;
        var items = me.callParent(arguments);
        items.push({
            id: me.getId() + "-updates",
            xtype: "button",
            text: _("Updates"),
            scope: me,
            icon: "images/refresh.png",
            menu: [{
                text: _("update"),
                icon: "images/refresh.png",
                handler: Ext.Function.bind(me.onCommandButton, me, [ "update" ])
            },{
                text: _("omv-update"),
                icon: "images/refresh.png",
                handler: Ext.Function.bind(me.onCommandButton, me, [ "omv-update" ])
            },{
                text: _("upgrade"),
                icon: "images/refresh.png",
                handler: Ext.Function.bind(me.onCommandButton, me, [ "upgrade" ])
            },{
                text: _("dist-upgrade"),
                icon: "images/refresh.png",
                handler: Ext.Function.bind(me.onCommandButton, me, [ "dist-upgrade" ])
            },{
                text: _("apt clean"),
                icon: "images/erase.png",
                handler: Ext.Function.bind(me.onCommandButton, me, [ "clean" ])
            }]
        },{
            id: me.getId() + "-backports",
            xtype: "button",
            text: _("Backports"),
            scope: me,
            icon: "images/software.png",
            menu: [{
                text: _("Enable Backports"),
                icon: "images/led_green.png",
                handler: Ext.Function.bind(me.onBackportsButton, me, [ "YES" ])
            },{
                text: _("Disable Backports"),
                icon: "images/led_red.png",
                handler: Ext.Function.bind(me.onBackportsButton, me, [ "NO" ])
            }]
        });
        return items;
    },

    getFormItems: function () {
        var me = this;
        return [{
            xtype: "fieldset",
            title: _("General settings"),
            fieldDefaults: {
                labelSeparator: ""
            },
            items: [{
                xtype: "checkbox",
                name: "testing",
                fieldLabel: _("Testing repo"),
                checked: false
            },{
                xtype: "checkbox",
                name: "extras",
                fieldLabel: _("Extras repo"),
                checked: false
            },{
                xtype: "textfield",
                name: "backportsStatus",
                fieldLabel: _("Backports"),
                submitValue: false
            }]
        },{
            xtype: "fieldset",
            title: _("Docker"),
            fieldDefaults: {
                labelSeparator: ""
            },
            items: [{
                xtype: "textfield",
                name: "dockerStorage",
                fieldLabel: _("Docker Storage"),
                allowBlank: true,
                value: "/var/lib/docker",
                plugins: [{
                    ptype: "fieldinfo",
                    text: _("Path to Docker images and containers storage. Leave blank to use custom /etc/docker/daemon.json.")
                }]
            },{
                xtype: "button",
                name: "installDocker",
                text: _("Install Docker"),
                scope: this,
                handler: Ext.Function.bind(me.onCommandButton, me, [ "docker_install" ]),
                margin: "0 5 5 0"
            },{
                xtype: "button",
                name: "removeDocker",
                text: _("Remove Docker"),
                scope: this,
                handler: Ext.Function.bind(me.onCommandButton, me, [ "docker_remove" ]),
                margin: "0 0 5 0"
            },{
                xtype: "textfield",
                name: "dockerStatus",
                fieldLabel: _("Status"),
                submitValue: false
            },{
                xtype: "textfield",
                name: "dockerVersion",
                fieldLabel: _("Version"),
                submitValue: false
            },{
                border: false,
                html: "<ul>" +
                        "<li>" + _("Install Docker will install the docker-compose package. Location: /usr/bin/ on all systems.") + "</li>" +
                        "<li>" + _("Install Docker will download and place a newer docker-compose in /usr/local/bin/ on 64 bit x86 systems only.") + "</li>" +
                        "<li>" + _("Install Docker will restart the docker service if daemon.json changes.") + "</li>" +
                        "<li>" + _("Remove Docker will remove the docker-compose package.") + "</li>" +
                        "<li>" + _("Remove Docker will delete docker-compose from /usr/local/bin/.") + "</li>" +
                      "</ul>"
            }]
        },{
            xtype: "fieldset",
            title: _("Portainer"),
            fieldDefaults: {
                labelSeparator: ""
            },
            items: [{
                xtype: "button",
                name: "installPortainer",
                text: _("Install Portainer"),
                scope: this,
                handler: Ext.Function.bind(me.onCommandButton, me, [ "portainer_install" ]),
                margin: "0 5 5 0"
            },{
                xtype: "button",
                name: "removePortainer",
                text: _("Remove Portainer"),
                scope: this,
                handler: Ext.Function.bind(me.onCommandButton, me, [ "portainer_remove" ]),
                margin: "0 5 5 0"
            },{
                xtype: "button",
                name: "showPortainer",
                text: _("Open Portainer web interface"),
                scope: this,
                handler: function() {
                    window.open("http://" + location.hostname + ":9000", "_blank");
                },
                margin: "0 0 5 0"
            },{
                xtype: "textfield",
                name: "portainerStatus",
                fieldLabel: _("Status"),
                submitValue: false
            },{
                border: false,
                html: "<ul>" +
                        "<li>" + _("Install Portainer will install the docker-ce package if not already installed.") + "</li>" +
                        "<li>" + _("Install Portainer will update Portainer to the latest image if the image already exists.") + "</li>" +
                        "<li>" + _("Portainer will listen on port 9000 for the web interface and 8000 for the agent.") + "</li>" +
                        "<li>" + _("Remove Portainer will remove the Portainer image and container but the volume will not be removed.") + "</li>" +
                      "</ul>"
            }]
        },{
            xtype: "fieldset",
            title: _("Cockpit"),
            fieldDefaults: {
                labelSeparator: ""
            },
            items: [{
                xtype: "button",
                name: "installCockpit",
                text: _("Install Cockpit"),
                scope: this,
                handler: Ext.Function.bind(me.onCommandButton, me, [ "cockpit_install" ]),
                margin: "0 5 5 0"
            },{
                xtype: "button",
                name: "removeCockpit",
                text: _("Remove Cockpit"),
                scope: this,
                handler: Ext.Function.bind(me.onCommandButton, me, [ "cockpit_remove" ]),
                margin: "0 5 5 0"
            },{
                xtype: "button",
                name: "showCockpit",
                text: _("Open Cockpit web interface"),
                scope: this,
                handler: function() {
                    window.open("http://" + location.hostname + ":9090", "_blank");
                },
                margin: "0 0 5 0"
            },{
                xtype: "textfield",
                name: "cockpitStatus",
                fieldLabel: _("Status"),
                submitValue: false
            },{
                xtype: "textfield",
                name: "cockpitVersion",
                fieldLabel: _("Version"),
                submitValue: false
            },{
                border: false,
                html: "<ul>" +
                        "<li>" + _("Cockpit listens on port 9090 for the web interface.") + "</li>" +
                      "</ul>"
            }]
        }];
    },

    onCommandButton: function(command) {
        var me = this;
        me.doSubmit();
        var msg = "";
        switch (command) {
            case "update":
                msg = _("Running apt-get update ...");
                break;
            case "omv-update":
                msg = _("Running omv-update ...");
                break;
            case "upgrade":
                msg = _("Running apt-get upgrade ...");
                break;
            case "dist-upgrade":
                msg = _("Running apt-get dist-upgrade ...");
                break;
            case "clean":
                msg = _("Running omv-aptclean ...");
                break;
            case "cockpit_install":
            case "cockpit_remove":
            case "docker_install":
            case "docker_remove":
            case "portainer_install":
            case "portainer_remove":
                str = command.split("_");
                if (str[1] == "remove") {
                  action = _("Removing");
                } else {
                  action = _("Installing");
                }
                msg = action + " " + str[0] + " ...";
                break;
        }
        var wnd = Ext.create("OMV.window.Execute", {
            title: msg,
            rpcService: "OmvExtras",
            rpcMethod: "doCommand",
            rpcParams: {
                "command": command
            },
            rpcIgnoreErrors: true,
            hideStartButton: true,
            hideStopButton: true,
            listeners: {
                scope: me,
                finish: function(wnd, response) {
                    wnd.appendValue(_("Done."));
                    wnd.setButtonDisabled("close", false);
                },
                exception: function(wnd, error) {
                    OMV.MessageBox.error(null, error);
                    wnd.setButtonDisabled("close", false);
                },
                close: function() {
                    if (command == "clean") {
                        document.location.reload();
                    } else {
                        me.doReload();
                    }
                }
            }
        });
        wnd.setButtonDisabled("close", true);
        wnd.show();
        wnd.start();
    },

    onBackportsButton: function(command) {
        var me = this;
        var msg = "";
        if (command == "NO") {
            msg = _("Disabling backports repo ...");
        } else {
            msg = _("Enabling backports repo ...");
        }
        OMV.MessageBox.wait(null, msg);
        OMV.Rpc.request({
            scope: me,
            relayErrors: false,
            rpcData: {
                service: "OmvExtras",
                method: "doBackports",
                params: {
                    "command": command
                }
            },
            success: function(id, success, response) {
                me.doReload();
                OMV.MessageBox.hide();
            }
        });
    }
});

OMV.WorkspaceManager.registerPanel({
    id: "repos",
    path: "/system/omvextras",
    text: _("Settings"),
    position: 10,
    className: "OMV.module.admin.system.omvextras.Repos"
});
