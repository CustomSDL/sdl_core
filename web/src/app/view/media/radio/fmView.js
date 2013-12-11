/**
 * @name MFT.FMView
 *
 * @desc Media FM View module visual representation
 *
 * @category	View
 * @filesource	app/view/media/radio/FMView.js
 * @version		2.0
 *
 * @author		Igor Zhavoronkin
 */
MFT.FMView = Em.ContainerView.create(MFT.LoadableView,{

    /** View Id */
    elementId: 'fm_view',

    childViews: ['fmInfo','rightmenu'],

    /** Class Name*/
    classNames: ['hidden','fm-info'],


    fmInfo: Em.ContainerView.create({

        elementId: 'fm-info-view',

        childViews: ['divider', 'band', 'infoWrapper'],

        infoWrapper: Em.ContainerView.create({

            classNames: 'info_wrapper',

            classNameBindings: 'view.controler.directKeypressed:hidden_display',

            childViews: ['wrapper', 'btnTag', 'tagStorePopup'],

            controlerBinding: 'MFT.MediaController',

            wrapper: Em.View.create({
                classNames: 'info_wrapper',
                controlerBinding: 'MFT.MediaController',
                HDChannels: function () {
                    var list = [],
                        i;

                    for (i = 1; i <= this.controler.get('currentActiveData').HDChannels; i++) {
                        list.push(
                            {
                                title: i,
                                active: (this.controler.get('currentActiveData').currentHDChannel == i)
                            }
                        );
                    }

                    return list;
                }.property('this.controler.currentActiveData.HDChannels'),
                template:  Em.Handlebars.compile(
                    '<div class="hdInfo hidden_display" {{bindAttr class="view.controler.currentActiveData.isHd:visible_display"}}>'+
                        '<div class="HDChannels">' +
                        '{{#each view.HDChannels}}' +
                            '<span {{bindAttr class="active:active"}}>{{title}}</span>'+
                        '{{/each}}'+
                            '<span class="type"> - WDVD</span>'+
                        '</div>'+
                        '<div class="hdtitle">{{view.controler.currentActiveData.title}}</div>'+
                        '<div class="hdartist">{{view.controler.currentActiveData.artist}}</div>'+
                        '</div>'+
                        '<div class="nonHDsonginfo not-visible visible" {{bindAttr class="view.controler.fmRDSTextIsHidden::visible"}}>{{view.songInfo}}</div>'+
                        '<span class="STAName not-visible visible" {{bindAttr class=" view.controler.fmRDSTextIsHidden::visible"}}>STA-{{view.STAName}}</span>'+
                        '<div class="fmgenre not-visible visible" {{bindAttr class="view.controler.genreIsHidden::visible "}}>{{view.controler.currentActiveData.genre}}</div>'
                ),
                /** Formate Station name according to HD or non HD State of Station*/
                STAName: function(){
                    if(this.controler.get('currentActiveData'))
                        return this.controler.get('currentActiveData').frequency.toString().replace('.','');
                }.property('controler.currentActiveData'),

                /** Formate Song name according to HD or non HD State of Station*/
                songInfo: function() {
                    var songInfo = this.controler.get('currentActiveData');

                    if (!(songInfo.title && songInfo.artist)) {
                        if (songInfo.title) {
                            return songInfo.title;
                        } else if (songInfo.artist) {
                            return songInfo.artist;
                        }
                    } else {
                        return songInfo.title + ' - ' + songInfo.artist;
                    }
                }.property('controler.currentActiveData')
            }),

            btnTag: MFT.Button.create({
                elementId: 'btn-tag',
                classNames: ['btn_tag'],
                classNameBindings: 'MFT.MediaController.isTagStoredDisabled::hidden',
                target:			   'MFT.MediaController',
                action:			   'tagStore',
                onDown: false,
                icon: 'images/media/store-ico.png',
                textBinding: 'MFT.locale.label.view_media_fm_tag'
            }),

            tagStorePopup: Em.ContainerView.create({
                classNames: 'popup',
                elementId: 'tagStorePopup',

                childViews: [
                    'window'
                ],

                window: Em.ContainerView.create({
                    classNames: 'popup-window',
                    elementId: 'tagStorePopupWindow',
                    childViews: [
                        'label',
                        'form'
                    ],

                    label: MFT.Label.create({
                        elementId: 'tagStorePopupLabel',
                        classNameBindings:'MFT.MediaController.isFullMemory:hidden',
                        contentBinding: 'MFT.MediaController.popUpMessage'
                    }),

                    form: Em.ContainerView.create({
                        classNameBindings:'MFT.MediaController.isFullMemory::hidden',
                        childViews: [
                            'label',
                            'delTagsButton',
                            'cancelButton'
                        ],

                        label: Em.View.create({
                            elementId: 'tagStorePopupFormLabel',

                            template:  Em.Handlebars.compile(
                                '<p>' + MFT.locale.label.view_media_fm_tags_memoryIsFull + '</p>' +
                                    '<p>' + MFT.locale.label.view_media_fm_tags_pleaseConnectIPod + '</p>'
                            )
                        }),

                        delTagsButton: MFT.Button.create({
                            elementId: 'tagStorePopupDelTagsButton',
                            target: 'MFT.MediaController',
                            action: 'deleteAllTags',
                            text: MFT.locale.label.view_media_fm_tags_deleteAllTags,
                            templateName: 'text'
                        }),

                        cancelButton: MFT.Button.create({
                            elementId: 'tagStorePopupCancelButton',
                            target: 'MFT.MediaController',
                            action: 'hideTagStorePopup',
                            text: MFT.locale.label.view_media_fm_tags_cancel,
                            templateName: 'text',
                            onDown: false
                        })
                    })
                })
            })

        }),

        divider: Em.View.create({
            classNames: 'divider_o'
        }),

        band: Em.View.create({
            elementId: 'band',

            controllerBinding: 'MFT.MediaController',

            template:  Em.Handlebars.compile(
                '{{#if view.controller.directKeypressed}}'+
                    '{{view.controller.directTuneToString}}'+
                    '{{else}}'+
                    '{{view.controller.currentActiveData.frequency}}'+
                    '{{#if view.controller.currentActiveData.isHd}}' +
                        '-{{view.controller.currentActiveData.currentHDChannel}}' +
                    '{{/if}}'+
                    '{{/if}}'
            )
        })
    }),

    rightmenu: Em.ContainerView.create({
        /** View Id */
        elementId: 'fm_rightmenu_view',
        /** Class Names */
        classNames: ['right-stock'],

        classNameBindings: 	[
            'MFT.States.media.radio.fm.options.active:hide-important'
        ],

        /** View Components*/
        childViews: [
            'hdButton',
            'scanButton',
            'optionsButton',
            'directTuneButton',
            'sendRequestButton',
            'tuneButtons'
        ],

        hdButton:  MFT.Button.extend({
            elementId: 'media_fm_hdButton',
            classNameBindings: [
                'MFT.MediaController.isHDActive:isHd',
                'MFT.MediaController.isHDButtonActiveInverted:replay_button_help',
                'MFT.helpMode: hide_icon'
            ],

            classNames:			['rs-item','helpmode_box_shadow'],
            target:			    'MFT.MediaController',
            action:			    'turnHdHelpVideoOn',
            text:		   MFT.locale.label.view_media_hdradio,
            disabled:			true,
            onDown: false,
            icon: 'images/media/passiv_horiz_led.png',
            // Change Icon for HD State
//            onIconChange: function(){
//                if(MFT.MediaController.get('isHDActive') && MFT.SettingsModel.isEnglish){
//                    this.set('icon', 'images/media/active_horiz_led.png');
//                }else{
//                    this.set('icon', 'images/media/passiv_horiz_led.png');
//                }
//            }.observes('MFT.MediaController.isHDActive'),

            disabledBinding: 'MFT.MediaController.isHDButtonActive'

        }),

        scanButton:  MFT.Button.extend({
            elementId:		'media_fm_scanButton',
            classNameBindings: ['MFT.helpMode:scan_button_help'],
            classNames:		['rs-item','helpmode_box_shadow'],
            icon:			  'images/media/passiv_horiz_led.png',
            // Change Icon for Frequency Scan
            onIconChange: function(){
                if(MFT.MediaController.isFrequencyScan){
                    this.set('icon', 'images/media/active_horiz_led.png');
                }else{
                    this.set('icon', 'images/media/passiv_horiz_led.png');
                }
            }.observes('MFT.MediaController.isFrequencyScan'),
            target:			'MFT.MediaController',
            action:			'turnFrequencyScan',
            onDown: 			false,
            text: 	   MFT.locale.label.view_media_scan
        }),

        optionsButton: MFT.Button.extend({
            elementId:	'media_fm_optionButton',
            classNames:			['rs-item'],
            icon:				'images/media/active_arrow.png',
            text:		   MFT.locale.label.view_media_options,
            action:			  'turnOnOptions',
            target:    'MFT.MediaController',
            disabledBinding:  'MFT.helpMode',
            // Define button template
            template: Ember.Handlebars.compile(
                '{{#with view}}'+
                    '<img class="ico" {{bindAttr src="icon"}} />'+
                    '<span>{{text}}</span>'+
                    '{{/with}}'
            )
        }),

        directTuneButton: MFT.Button.extend({
            elementId:	'media_fm_directButton',
            classNameBindings: ['MFT.helpMode:direct_button_help'],
            templateName:	'text',
            classNames:			['rs-item','helpmode_box_shadow'],
            target:				'MFT.MediaController',
            action:				'onDirectTune',
            onDown: false,
            text: function(){
                if(!MFT.MediaController.directTuneHide){
                    this.set('action','offDirectTune');
                    return MFT.locale.label.view_media_presets
                }else{
                    this.set('action','onDirectTune');
                    return MFT.locale.label.view_media_directTune
                }
            }.property('MFT.MediaController.directTuneHide','MFT.locale.label')
        }),

        sendRequestButton: MFT.Button.extend({
            elementId:	'media_fm_sendRequestButton',
            classNames:			['rs-item'],
            target:				'MFT.AppController',
            action:				'sendAccessRequest',
            onDown: false,
            templateName: 'text',
            text: function(){
                if(MFT.AppController.sdlAccessStatus){
                    return MFT.locale.label.view_media_cancelAccess;
                } else {
                    return MFT.locale.label.view_media_grantAccess;
                }
            }.property('MFT.AppController.sdlAccessStatus'),
            disabled: function() {
                return (MFT.AppController.sdlControlStatus == 2);
            }.property('MFT.AppController.sdlControlStatus')
        }),

        tuneButtons: Em.ContainerView.extend({
            elementId: 'media_fm_tuneButtons',

            childViews: [
                'tuneDown',
                'tuneUp'
            ],

            tuneUp: MFT.Button.extend({
                elementId:	'media_fm_tuneUpButton',
                target:				'MFT.MediaController',
                action:				'tuneUp',
                onDown: false,
                templateName: 'text',
                text: '>>'
            }),

            tuneDown: MFT.Button.extend({
                elementId:	'media_fm_tuneDownButton',
                target:				'MFT.MediaController',
                action:				'tuneDown',
                onDown: false,
                templateName: 'text',
                text: '<<'
            })
        })
    })

});