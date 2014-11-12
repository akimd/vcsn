require.config({paths: {d3: "//d3js.org/d3.v3.min"}});
require(['widgets/js/widget','d3','jquery','underscore'], function(WidgetManager, d3, $,  _){
    var AutomatonView = IPython.DOMWidgetView.extend({

        // RENDER THE VIEW
        render: function(){
            // Here we define a variable with this so we can call from everywhere

            // Attributs of the SVG
            this.width =960, this.height = 500;

            // disable some key shorcut from notebook
            var cmd = IPython.keyboard_manager.command_shortcuts;
            cmd.remove_shortcut('i');
            cmd.remove_shortcut('l');
            cmd.remove_shortcut('f');
            cmd.remove_shortcut('ctrl');
            cmd.remove_shortcut('s');

            // Render the svg
            this.svg = d3.select(this.el).append('svg')
                .attr('width', this.width)
                .attr('height', this.height)
                .style('userSelect' , "none")
                .style('webkiUserSelect' , "none")
                .style('MozUserSelect' , "none")
                .style('oUserSelect', 'none')
                .style('msUserSelect', 'none')
                .style('draggable', 'false')
                .classed('ctrl', false)
                .style('cursor', 'crosshair')

            // Define mouse and keyboard variables
            this.selected_state = null,
            this.selected_link = null,
            this.mousedown_link = null,
            this.mousedown_state = null,
            this.mouseup_state = null;
            this.lastKeyDown = -1;

            // Initialize datas
            this.states = this.model.get('states');
            var trans = this.model.get('transitions');
            this.transitions = this.completeTransitions(trans);
            this.lastStateId = this.states[this.states.length - 1].id;

            // Define path and circle here to avoid recreation of svg in every update
            this.path = this.svg.append('svg:g').selectAll('g');
            this.circle = this.svg.append('svg:g').selectAll('g');

            // frame with editable text for labels
            this.frame = this.svg.append('foreignObject')
                                .attr('x', 0)
                                .attr('y', 0)
                                .attr("width", 800)
                                .attr("height", 50)
                                .append("xhtml:form")
                                .attr('class', 'frame')
                                 .append('input');

            // Lines displayed when draggind new states
            this.drag_line = this.svg.append('svg:path')
                .style('fill', 'none')
                .style('stroke', '#000')
                .style('stroke-width', '1.50px')
                .attr('class', 'transition dragline hidden')
                .attr('d', 'M0,0L0,0');

            // Arrow marker for transitions
            this.svg.append('svg:defs').append('svg:marker')
                .attr('id', 'end-arrow')
                .attr('viewBox', '-3 -10 20 20')
                .attr('refX', 2)
                .attr('markerWidth', 6)
                .attr('markerHeight', 6)
                .attr('orient', 'auto')
              .append('svg:path')
                .attr('d', 'M-3,12 l22,-12 l-22,-12 l7,12 Z')
                .attr('fill', '#000');

            // Variable using in keyboard functions
            this.overtext = false;

            // Closure
            var that = this;

            // catch source and target state with their ID
            for(var c = 0; c <= this.transitions.length -1; c++)
                {
                    this.transitions[c].source = this.idToState(this.transitions[c].source);
                    this.transitions[c].target = this.idToState(this.transitions[c].target);
                }

            // The force layout for physical simulation
            this.force = d3.layout.force()
                .nodes(this.states)
                .links(this.transitions)
                .size([this.width, this.height])
                .linkDistance(function(d) {
                    if(that.isShort(d)) {return 75;}
                    else { return 175;}
                })
                .charge(function(d) {
                    if(that.isHidden(d)) {return -100;}
                    else {return -750;}
                })
                .on('tick', function(d,i){ that.stepForce(this, d, i);})

            // Listen for mouse and keyboard events on the whole document
            this.svg.on('mousedown', function(d,i){ that.mousedown(this, d, i);})
                .on('mousemove', function(d,i){ that.mousemove(this, d, i);})
                .on('mouseup', function(d,i){ that.mouseup(this, d, i);});
            d3.select(window)
                .on('keydown', function(d,i){ that.keydown(this, d, i);})
                .on('keyup', function(d,i){ that.keyup(this, d, i);});
            this.hackUpdate();
        },

        hackUpdate: function(){
          // Little hack to update model without touch the view
            //console.log('before splice', this.transitions);
            //this.transitions.splice(this.transitions[this.transitions.length - 1],1);
            //this.model.set('transitions', this.transitions);
            //this.touch();
            //this.transitions = this.model.get('transitions');
            // Push
            console.log('enter to the hack');
            var old_transitions = this.transitions
            var transition = {'source': '0', 'target':'0', 'label': ''}
            var new_transitions = this.model.get('transitions').slice();
            new_transitions.push(transition);
            this.model.set('transitions', new_transitions);
            this.touch();

            console.log('before splice', this.model.get('transitions'))
            // Splice the push
            //var new_transitions2 =
            //console.log('new_trans2', new_transitions2);
            //new_transitions2.splice(new_transitions2[this.transitions.length -1],1);
            this.model.set('transitions', old_transitions);
            this.touch();
            console.log('after splice', this.model.get('transitions'))

            this.update();
        },

        // Catch a state by his ID
        idToState: function(id){
            for(var j=0; j <= this.states.length -1; j++)
                {
                    if(this.states[j].id == id)
                        {
                            return this.states[j];
                        }
                }
        },

         stepForce: function(doc, da, i) {
             var that = this;
            // draw transitions with proper padding from state centers
            this.path.selectAll('path').attr('d', function(d) {
                if((d.target != d.source) && ((that.existingTrans(d,that.transitions)) == false)) {
                    var deltaX = d.target.x - d.source.x ,
                        deltaY = d.target.y - d.source.y,
                        dist = Math.sqrt(deltaX * deltaX + deltaY * deltaY),
                        normX = deltaX / dist,
                        normY = deltaY / dist,
                        sourcePadding = 20,
                        targetPadding = 29,
                        sourceX = d.source.x + (sourcePadding * normX),
                        sourceY = d.source.y + (sourcePadding * normY),
                        targetX = d.target.x - (targetPadding * normX),
                        targetY = d.target.y - (targetPadding * normY);
                    return 'M' + sourceX + ',' + sourceY + 'L' + targetX + ',' + targetY;
                }
                // for double transitions
                else if((that.existingTrans(d,that.transitions)) == true){
                     var deltaX = d.target.x - d.source.x ,
                        deltaY = d.target.y - d.source.y,
                        dist = Math.sqrt(deltaX * deltaX + deltaY * deltaY),
                        normX = deltaX / dist,
                        normY = deltaY / dist,
                        sourcePadding = 20,
                        targetPadding = 29,
                        sourceX = d.source.x + (sourcePadding * normX),
                        sourceY = d.source.y + (sourcePadding * normY),
                        targetX = d.target.x - (targetPadding * normX),
                        targetY = d.target.y - (targetPadding * normY),
                        mx = ((d.target.x - d.source.x)/2),
                        my = ((d.target.y - d.source.y)/2),
                        distx = (d.target.x - d.source.x),
                        disty = (d.target.y - d.source.y);
                    //return "M" + (d.source.x) + ',' + (d.source.y) + " Q 50,50" + (d.target.x) + (d.target.y);
                    return "M" + (sourceX)  +',' + (sourceY) + "q" + ((-disty*0.2 + distx/2)) + ',' + ((distx*0.2 + disty/2)) + " " + (targetX - sourceX) + ',' + (targetY - sourceY);
                }
                else{ // nice cubic bezier curve for the loop ( transition with same source and target)
                    return "M" + (d.source.x-2)  +',' + (d.source.y-18) + " c -50,-50 50,-50 18,-9  ";
                }
            });
            // draw states from proper position
            this.circle.attr('transform', function(d) {
                return 'translate(' + d.x + ',' + d.y + ')';
            });
            // draw label horizontally on transitions with proper position
            this.svg.selectAll("text.transitionlabelholder").attr('transform', function(d) {
                if((d.target != d.source) && ((that.existingTrans(d,that.transitions)) == false)) {
                    return 'translate(' + (d.target.x - (d.target.x - d.source.x)/2) + ',' + ((d.target.y - (d.target.y - d.source.y)/2)-8) + ')';
                }
                else if((that.existingTrans(d,that.transitions)) == true){
                    var deltaX = d.target.x - d.source.x ,
                        deltaY = d.target.y - d.source.y,
                        dist = Math.sqrt(deltaX * deltaX + deltaY * deltaY),
                        normX = deltaX / dist,
                        normY = deltaY / dist,
                        sourcePadding = 20,
                        targetPadding = 29,
                        sourceX = d.source.x + (sourcePadding * normX),
                        sourceY = d.source.y + (sourcePadding * normY),
                        targetX = d.target.x - (targetPadding * normX),
                        targetY = d.target.y - (targetPadding * normY),
                        mx = ((d.target.x - d.source.x)/2),
                        my = ((d.target.y - d.source.y)/2),
                        distx = (d.target.x - d.source.x),
                        disty = (d.target.y - d.source.y);
                   return 'translate(' + (sourceX + (-disty*0.18 + distx/2)) + ',' + (sourceY + (distx*0.18 + disty/2)) + ')';
                }
                else { // label on loops
                    return 'translate(' + (d.source.x -5) + ',' + (d.target.y - 60) + ')';
                }
            });
        },

        // MOUSE FUNCTIONS
        resetMouseVars: function(){
            this.mousedown_state = null;
            this.mouseup_state = null;
            this.mouseover_state = null;
            this.mousedown_transition = null;
        },
        mousedown: function(doc, data, id){
            d3.event.preventDefault();
            this.svg.classed('active', true);
            if(d3.event.ctrlKey || this.mousedown_state || this.mousedown_transition) return;
            if(this.svg.classed('ctrl') == false) {
            // insert new state at point
            var point = d3.mouse(doc);
            var lastState = this.lastStateValidId(this.states);
            var state = {id: lastState + 1};
            state.x = point[0];
            state.y = point[1];
            var new_states = this.model.get('states').slice();
            new_states.push(state);
            this.model.set('states', new_states);
            this.touch();
            this.states = new_states;
            this.model.set("lastStateId", this.lastStateId);
            this.touch();
            }
            this.update();
        },
        mousemove: function(doc, data, id){
            d3.event.preventDefault();
            if(!this.mousedown_state) return;
            // update drag line
            if(this.svg.classed('ctrl') == false)
                this.drag_line.attr('d', 'M' + this.mousedown_state.x + ',' + this.mousedown_state.y + 'L' + d3.mouse(doc)[0] + ',' + d3.mouse(doc)[1]);
            this.update();
        },
        mouseup: function(doc, data, id){

            // add new transition when we are dragging from a state
            if(this.mousedown_state && this.mouseover_state) {
                this.drag_line.classed('hidden', true)
                    .style('marker-end','');
                this.mouseup_state = this.mouseover_state;
                if(this.mouseup_state === this.mousedown_state) { this.resetMouseVars(); return; }

                // add transition and update if already exist
                var source, target;
                    source = this.mousedown_state;
                    target = this.mouseup_state;
                var transition;
                transition = this.transitions.filter(function(l) {
                    return (l.source === source && l.target === target);
                })[0];

                if(!this.isHidden(this.mousedown_state)) // a transparent state can't have other transitions
                {
                    if(!transition) {
                        transition = {source: source, target: target, label: 'b'};
                        var new_transitions = this.model.get('transitions').slice();
                        new_transitions.push(transition);
                        this.model.set('transitions', new_transitions)
                        this.touch();
                    }
                }

                // Reset variable and select the new transition
                this.selected_transition = transition;
                this.selected_state = null;
                this.mousedown_state = null;
                this.mouseup_state = null;
                this.mousedown_transition = null;
                this.update();
            }

            // because :active only works in WebKit
            this.svg.classed('active', false);
            this.drag_line.classed('hidden', true);
            // clear mouse event vars
            this.resetMouseVars();
        },

        // KEYBOARD FUNCTIONS
        spliceTransitionsForState: function(state, transitions){
            var toSplice = transitions.filter(function(l) {
                return (l.source === state || l.target === state);
            });
            toSplice.map(function(l) {
                transitions.splice(transitions.indexOf(l), 1);
            });
        },

        keydown: function(doc, d , i){

            if(this.lastKeyDown !== -1) return;
            this.lastKeyDown = d3.event.keyCode;

            // ctrl
            if(d3.event.keyCode === 17) {
                this.circle.call(this.force.drag);
                //this.drag_line.classed('hidden', false);
                this.svg.classed('ctrl', true);
            }

            if(!this.selected_state && !this.selected_transition) return;
            switch(d3.event.keyCode) {
                case 8: // backspace
                case 66: // key delete
                    if(!this.overtext){
                        if(this.selected_state) {
                            this.states.splice(this.states.indexOf(this.selected_state), 1);
                            this.spliceTransitionsForState(this.selected_state, this.transitions);
                        }
                        else if(this.selected_transition && !this.overtext) {
                            if(this.selected_transition.target == this.selected_transition.source){
                                this.transitions.splice(this.transitions.indexOf(this.selected_transition), 1);
                            }
                            else {
                                this.transitions.splice(this.transitions.indexOf(this.selected_transition), 1);
                            }
                        }
                        if(!this.overtext) {
                            this.selected_transition = null;
                            this.selected_state = null;
                        }
                        this.update();
                        break;
                    }
                case 83: // key S : fix a node
                    if(!this.overtext){
                        if(this.selected_state) {
                            console.log('fix the state', this.selected_state);
                            this.selected_state.fixed = true;
                        }
                    }
                    this.update();
                    break;
                case 70: // key F : create final transition
                    if(!this.overtext){
                        if(this.selected_state) {
                            if(!(this.isFinal(this.selected_state, this.transitions))) {
                                var pointx = this.selected_state.x,
                                    pointy = this.selected_state.y,
                                    state = {id: (this.selected_state.id) + 0.2};
                                state.x = pointx +75;
                                state.y = pointy;
                                var new_states = this.model.get('states').slice();
                                new_states.push(state);
                                this.model.set('states', new_states);
                                this.touch();
                                var transition = {source: this.selected_state, target: state, label: ' '};
                                var new_transitions = this.model.get('transitions').slice();
                                new_transitions.push(transition);
                                this.model.set('transitions', new_transitions);
                                this.touch();
                            }
                            else { // TO IMPROVE
                                this.spliceTransitionsForState(this.findFinalState(this.selected_state, this.states), this.transitions);
                                this.states.splice(this.states.indexOf(this.findFinalState(this.selected_state, this.states)), 1);
                            }
                        }
                    this.update();
                    break;
                    }
                case 73: // key I : create initial transition
                    if(!this.overtext){
                        if(this.selected_state) {
                            if(!(this.isInitial(this.selected_state, this.transitions))) {
                                var pointx = this.selected_state.x,
                                    pointy = this.selected_state.y,
                                    state = {id: (this.selected_state.id) + 0.1};
                                state.x = pointx -75;
                                state.y = pointy;
                                var new_states = this.model.get('states').slice();
                                new_states.push(state);
                                this.model.set('states', new_states);
                                this.touch();
                                var transition = {source: state, target: this.selected_state, label: ' '};
                                var new_transitions = this.model.get('transitions').slice();
                                new_transitions.push(transition);
                                this.model.set('transitions', new_transitions);
                                this.touch();
                            }
                            else {
                                this.spliceTransitionsForState(this.findInitialState(this.selected_state, this.states), this.transitions);
                                this.states.splice(this.states.indexOf(this.findInitialState(this.selected_state, this.states)), 1);
                            }
                        }
                    this.update();
                    break;
                    }
                case 76: // key L : create loop on node
                    if(!this.overtext){
                        if(this.selected_state && !(this.isLoop(this.selected_state, this.transitions))) {
                            var loop = {source: this.selected_state, target: this.selected_state, label:'a', short: false};
                            var new_transitions = this.model.get('transitions').slice();
                            new_transitions.push(loop);
                            this.model.set('transitions', new_transitions);
                            this.touch();
                        }
                    this.update();
                    break;
                    }
            }
        },

        keyup: function(doc, d, i){
              this.lastKeyDown = -1;
              // ctrl
              if(d3.event.keyCode === 17) {
                this.circle
                  .on('mousedown.drag', null)
                  .on('touchstart.drag', null);
                this.svg.classed('ctrl', false);
              }
        },

        existingTrans: function(transition, transitions){
            for(var i=0; i<= transitions.length -1; i++){
                if((transitions[i].source == transition.target) &&
                  (transitions[i].target == transition.source)){
                    return true;
                    }
                }
            return false;
        },

        lastStateValidId: function(states){
            for(var t = states.length -1; t>=0; t--)
                    {
                        if((parseFloat((states[t].id)) == parseInt((states[t].id))) && !isNaN((states[t].id)))
                            {
                                return states[t].id;
                            }
                    }

        },

        findFinalState: function(state, states){
            for(var i = 0; i <= states.length -1; i++)
                {
                    if(states[i].id.toString() == (state.id.toString() + '.2'))
                        {
                            return states[i];
                        }
                }
        },

        findInitialState: function(state, states){
            for(var i = 0; i <= states.length -1; i++)
                {
                    if(states[i].id.toString() == (state.id.toString() + '.1'))
                        {
                            return states[i];
                        }
                }
        },

        isInitial: function(state, transitions){
                for(var i=0; i<= transitions.length -1; i++)
                    {
                        if(transitions[i].source.id == state.id || transitions[i].target.id == state.id)
                            {
                                if(!((parseFloat((transitions[i].source.id)) == parseInt((transitions[i].source.id))) && !isNaN((transitions[i].source.id))) )
                                    {
                                        return true;
                                    }
                            }
                    }
                return false;
        },

        isFinal: function(state, transitions){
                for(var i=0; i<= transitions.length -1; i++)
                    {
                        if(transitions[i].source.id == state.id || transitions[i].target.id == state.id)
                            {
                                if(!((parseFloat((transitions[i].target.id)) == parseInt((transitions[i].target.id))) && !isNaN((transitions[i].target.id))) )
                                    {
                                        return true;
                                    }
                            }
                    }
                return false;
        },

        isHidden: function(state){
                if(!(parseFloat(state.id) == parseInt(state.id) && !isNaN(state.id)))
                    {
                        return true;
                    }
                else{ return false;}
        },

        isShort: function(transition){
                if(!((parseFloat((transition.target.id)) == parseInt((transition.target.id))) && !isNaN((transition.target.id)))
                  || !((parseFloat((transition.source.id)) == parseInt((transition.source.id))) && !isNaN((transition.source.id))))
                    {
                        return true;
                    }
                else {return false;}

        },

        isLoop: function(state, transitions){
                for(var i =0; i <= transitions.length -1; i++)
                    {
                        if(transitions[i].target.id == transitions[i].source.id)
                            {
                                if(transitions[i].target.id == state.id)
                                    {
                                        return true;
                                    }
                            }
                    }
                return false;
        },

        idToNode: function(id){
            for(var j=0; j <= this.states.length -1; j++)
                {
                    if(this.states[j].id == id)
                        {
                            return this.states[j];
                        }
                }
        },

        completeTransitions: function(transitions){
                for(var i =0; i <= transitions.length -1; i++)
                    {
                        if(transitions[i].length != 3)
                            {
                                if(typeof(transitions[i].source) === 'undefined')
                                    {
                                        //(transitions[i].target)+'.1'
                                        var state = {id:((transitions[i].target)+'.1') };

                                        this.states.push(state);
                                        var trans = {source: state.id.toString() , target:transitions[i].target, label:transitions[i].label};
                                        transitions.splice(i,1);
                                        transitions.push(trans);
                                        i --;
                                        this.model.set('transitions', transitions);
                                        this.touch();
                                        this.model.set('states', this.states);
                                        this.touch();
                                    }
                                else if(typeof(transitions[i].target) === 'undefined')
                                    {
                                        //(transitions[i].source)+'.2'
                                        var state = {id:((transitions[i].source)+'.2')};
                                        this.states.push(state);
                                        var trans = {source: transitions[i].source, target: state.id.toString(), label:transitions[i].label}
                                        transitions.splice(i,1);
                                        transitions.push(trans);
                                        i --;
                                        this.model.set('transitions', transitions);
                                        this.touch();
                                        this.model.set('states', this.states);
                                        this.touch();
                                    }
                            }
                    }
            return transitions;

        },

        // UPDATE FUNCTION
        update: function(){

            var colors = d3.scale.category10();

            this.force.nodes(this.states)
                .links(this.transitions)

            // closure
            var that = this;

            // Update the datas on model
            this.states = this.model.get('states');
            this.transitions = this.model.get('transitions');

            // lastStateId is not a parameter, he is udpate automatically. But you can still access to it
            this.lastStateId = this.lastStateValidId(this.states);
            this.model.set("lastStateId", this.lastStateId);
            this.model.set('states', this.states);
            this.touch();


            this.frame.attr("onSubmit", function(){return false;})
                        .on("mouseover", function() {
                                     this.focus();
                                     that.overtext = true;
                        })
                        .attr("value", function() {
                                    if(that.selected_transition) { return that.selected_transition.label;}
                        })
                        .on("mouseout", function() {
                                    this.blur()
                                    that.overtext = false;
                                    if(that.selected_transition) {
                                        var txt = that.frame.node().value;
                                        that.selected_transition.label = txt;
                                        that.model.set('transitions', that.transitions);
                                        that.touch();
                                    }
                                    this.value = '';
                                    that.update();
                        });

            // Define elements groups and bound them data
            this.path = this.path.data(this.transitions, function(d) {return [d.source.id, d.target.id];});
            this.circle = this.circle.data(this.states, function(d) {return d.id;});

            // TRANSITIONS
            // Update existing transition
            this.path.selectAll('path')
                .classed('selected', function(d) {return d == that.selected_transition;})
                .style('marker-end', function(d) {return 'url(#end-arrow)'; })
                .style('stroke-dasharray', function(d) {
                    if (d == that.selected_transition) {return "10,2";}
                    else {return'';}})
            // Enter new transition
            var p = this.path.enter().append('svg:g');
            p.append('svg:path')
                .attr('class', 'transition')
                .style('fill', 'none')
                .style('stroke', '#000')
                .style('stroke-width', '1.75px')
                .style('cursor', 'pointer')
                .classed('selected', function(d) { return d === that.selected_transition; })
                .style('marker-end', function(d) {return 'url(#end-arrow)'; })
                .style('stroke-dasharray', function(d) {
                    if (d == that.selected_transition) {return "10,2";}
                    else {return'';}})
                .on('mousedown', function(d) {
                      if(d3.event.ctrlKey) return;
                      // select transition
                      that.mousedown_transition = d;
                      if(that.mousedown_transition === that.selected_transition) that.selected_transition = null;
                      else that.selected_transition = that.mousedown_transition;
                      that.selected_state = null;
                      that.update();
                })

            // Exit old transitions
            this.path.exit().remove();

            // TRANSITIONS LABELS
            // Update existing labels
            this.path.selectAll('text')
                .text(function(d){return d.label});
            // Show transition labels
            p.append('svg:text')
            .attr("class", "transitionlabelholder")
            .style("font-size", "18px")
            //.style("fill", "#00CCFF")
            .text(function(d) {return d.label});

            // STATES
            // Update existing states
            this.circle.selectAll('circle')
                .style('fill', function(d) {
                    if (that.isHidden(d)) {return "transparent";}
                    else {return (d === that.selected_state) ? d3.rgb(colors(d.id)).brighter().toString() : colors(d.id); }})
                .style('stroke', function(d) {
                    if (that.isHidden(d)) {return "transparent";}
                    else {return d3.rgb(colors(d.id)).darker().toString(); }})
            // Enter new states
            var g = this.circle.enter().append('svg:g')
            g.append('svg:circle')
                .attr('class', 'state')
                .attr('r', 20)
                .style('stroke-width', '1.5px')
                .style('cursor', 'pointer')
                .style('fill', function(d) {
                    if (that.isHidden(d)) {return "transparent";}
                    else {return (d === that.selected_state) ? d3.rgb(colors(d.id)).brighter().toString() : colors(d.id); }})
                .style('stroke', function(d) {
                    if (that.isHidden(d)) {return "transparent";}
                    else {return d3.rgb(colors(d.id)).darker().toString(); }})
                .on('mouseover', function(d) {
                    that.mouseover_state = d;
                    d3.select(this).attr('transform', function(d) {
                                        if(that.isHidden(d)) {return '';}
                                        else{return 'scale(1.5)';}})
                                   .style('stroke', function(d) {
                                        if(that.isHidden(d)) {return "black";}
                                        else{return d3.rgb(colors(d.id)).darker().toString();}});
                })
                .on('mouseout', function() {
                    d3.select(this).attr('transform', '')
                        .style('stroke', function(d) {
                            if(that.isHidden(d)) {return "transparent";}
                            else{return d3.rgb(colors(d.id)).darker().toString(); }});
                })
                .on('mousedown', function(d) {
                    d3.event.preventDefault();
                    if(d3.event.ctrlKey) return;
                    // select state
                    that.mousedown_state = d;
                    if(that.mousedown_state === that.selected_state) that.selected_state = null;
                    else {that.selected_state = that.mousedown_state;}
                    that.selected_transition = null;
                    // reposition drag line
                    console.log('ctlr', that.svg.classed('ctrl') == false)
                    if(!that.isHidden(that.mousedown_state)  && that.svg.classed('ctrl') == false) // a transparent state cannot have other transition and don't show when its on drag mode
                    {
                        that.drag_line
                            .style('marker-end', 'url(#end-arrow)')
                            .classed('hidden', false)
                            .attr('d', 'M' + that.mousedown_state.x + ',' + that.mousedown_state.y + 'L' + that.mousedown_state.x + ',' + that.mousedown_state.y);
                    }
                    that.update();
                })

            // Exit old states
            this.circle.exit().remove();

            // STATES ID
            // Update states ID
            this.circle.selectAll('text')
                .text(function(d) {return d.id;});
            // Show states ID
            g.append('svg:text')
                .attr('x', 0)
                .attr('y', 4)
                .attr('class', 'id')
            .style('font', '15px sans-serif')
                .style("text-anchor", "middle")
                .style("fill", function(d) {
                    if(that.isHidden(d)) return "transparent";
                    else return "white";})
                .text(function(d) { return d.id; });

            console.log('transitions', this.transitions);
            this.force.start();
        }
        // END UPDATE FUNCTION

    });
    WidgetManager.register_widget_view("AutomatonView", AutomatonView);
});
