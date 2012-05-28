/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Boss_Blackheart_the_Inciter
SD%Complete: 75
SDComment: Incite Chaos not functional since core lacks Mind Control support
SDCategory: Auchindoun, Shadow Labyrinth
EndScriptData */

#include "ScriptPCH.h"
#include "shadow_labyrinth.h"

#define SPELL_INCITE_CHAOS    33676
#define SPELL_INCITE_CHAOS_B  33684  // debuff applied to each member of party
#define SPELL_CHARGE          33709
#define SPELL_WAR_STOMP       33707

#define SAY_INTRO1          -1555008 // not used
#define SAY_INTRO2          -1555009 // not used
#define SAY_INTRO3          -1555010 // not used
#define SAY_AGGRO1          -1555011
#define SAY_AGGRO2          -1555012
#define SAY_AGGRO3          -1555013
#define SAY_SLAY1           -1555014
#define SAY_SLAY2           -1555015
#define SAY_HELP            -1555016 // not used
#define SAY_DEATH           -1555017

// below, not used
#define SAY2_INTRO1         -1555018
#define SAY2_INTRO2         -1555019
#define SAY2_INTRO3         -1555020
#define SAY2_AGGRO1         -1555021
#define SAY2_AGGRO2         -1555022
#define SAY2_AGGRO3         -1555023
#define SAY2_SLAY1          -1555024
#define SAY2_SLAY2          -1555025
#define SAY2_HELP           -1555026
#define SAY2_DEATH          -1555027

class boss_blackheart_the_inciter : public CreatureScript
{
    public:
        boss_blackheart_the_inciter() : CreatureScript("boss_blackheart_the_inciter") { }

        struct boss_blackheart_the_inciterAI : public ScriptedAI
        {
            boss_blackheart_the_inciterAI(Creature* c) : ScriptedAI(c)
            {
                _instance = c->GetInstanceScript();
            }

            void Reset()
            {
                _inciteChaos = false;
                _inciteChaosTimer = 20*IN_MILLISECONDS;
                _inciteChaosWaitTimer = 15*IN_MILLISECONDS;
                _chargeTimer = 5*IN_MILLISECONDS;
                _knockbackTimer = 15*IN_MILLISECONDS;

                if (_instance)
                    _instance->SetData(DATA_BLACKHEARTTHEINCITEREVENT, NOT_STARTED);
            }

            void KilledUnit(Unit* /*victim*/)
            {
                DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), me);
            }

            void JustDied(Unit* /*victim*/)
            {
                DoScriptText(SAY_DEATH, me);

                if (_instance)
                    _instance->SetData(DATA_BLACKHEARTTHEINCITEREVENT, DONE);
            }

            void EnterCombat(Unit* /*who*/)
            {
                DoScriptText(RAND(SAY_AGGRO1, SAY_AGGRO2, SAY_AGGRO3), me);

                if (_instance)
                    _instance->SetData(DATA_BLACKHEARTTHEINCITEREVENT, IN_PROGRESS);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                //if (spell->Id == SPELL_INCITE_CHAOS)
                    //me->CastSpell(target, SPELL_INCITE_CHAOS_B, true);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (_inciteChaos)
                {
                    if (_inciteChaosWaitTimer <= diff)
                    {
                        _inciteChaos = false;
                        _inciteChaosWaitTimer = 15*IN_MILLISECONDS;
                    }
                    else
                        _inciteChaosWaitTimer -= diff;

                    return;
                }

                if (_inciteChaosTimer <= diff)
                {
                    DoCast(me, SPELL_INCITE_CHAOS);
                    DoResetThreat();
                    _inciteChaos = true;
                    _inciteChaosTimer = 40*IN_MILLISECONDS;
                    return;
                }
                else
                    _inciteChaosTimer -= diff;

                // Charge Timer
                if (_chargeTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_CHARGE);
                    _chargeTimer = urand(15, 25) *IN_MILLISECONDS;
                }
                else
                    _chargeTimer -= diff;

                // Knockback Timer
                if (_knockbackTimer <= diff)
                {
                    DoCast(me, SPELL_WAR_STOMP);
                    _knockbackTimer = urand(18, 24) *IN_MILLISECONDS;
                }
                else
                    _knockbackTimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            InstanceScript* _instance;
            bool _inciteChaos;
            uint32 _inciteChaosTimer;
            uint32 _inciteChaosWaitTimer;
            uint32 _chargeTimer;
            uint32 _knockbackTimer;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_blackheart_the_inciterAI(creature);
        }
};

void AddSC_boss_blackheart_the_inciter()
{
    new boss_blackheart_the_inciter();
}
