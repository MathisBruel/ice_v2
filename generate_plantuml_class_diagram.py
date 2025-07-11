#!/usr/bin/env python3
"""
Générateur de diagrammes de classes PlantUML pour projets C++ CMake
Analyse le code source C++ et génère un diagramme UML complet avec relations
"""

import os
import re
import argparse
import json
from pathlib import Path
from typing import Dict, List, Set, Tuple, Optional
from dataclasses import dataclass, field
from enum import Enum


class AccessModifier(Enum):
    PUBLIC = "+"
    PRIVATE = "-"
    PROTECTED = "#"


class RelationType(Enum):
    INHERITANCE = "--|>"
    COMPOSITION = "*--"
    AGGREGATION = "o--"
    ASSOCIATION = "-->"
    DEPENDENCY = "..>"
    IMPLEMENTATION = "..|>"


@dataclass
class ClassMember:
    name: str
    type: str
    access: AccessModifier
    is_static: bool = False
    is_const: bool = False
    is_virtual: bool = False
    is_pure_virtual: bool = False
    default_value: Optional[str] = None


@dataclass
class ClassMethod:
    name: str
    return_type: str
    parameters: List[Tuple[str, str]]  # (type, name)
    access: AccessModifier
    is_static: bool = False
    is_const: bool = False
    is_virtual: bool = False
    is_pure_virtual: bool = False
    is_constructor: bool = False
    is_destructor: bool = False
    is_operator: bool = False


@dataclass
class ClassInfo:
    name: str
    namespace: str = ""
    is_abstract: bool = False
    is_interface: bool = False
    is_template: bool = False
    template_params: List[str] = field(default_factory=list)
    base_classes: List[str] = field(default_factory=list)
    members: List[ClassMember] = field(default_factory=list)
    methods: List[ClassMethod] = field(default_factory=list)
    file_path: str = ""
    dependencies: Set[str] = field(default_factory=set)
    forward_declarations: Set[str] = field(default_factory=set)


@dataclass
class Relationship:
    source: str
    target: str
    type: RelationType
    label: Optional[str] = None
    multiplicity: Optional[str] = None


class CppParser:
    def __init__(self, exclude_files: Optional[List[str]] = None):
        self.classes: Dict[str, ClassInfo] = {}
        self.relationships: List[Relationship] = []
        self.includes: Dict[str, Set[str]] = {}
        self.namespaces: Dict[str, str] = {}
        self.exclude_files = exclude_files or []
        
        # Patterns regex pour l'analyse
        self.class_pattern = re.compile(
            r'^\s*(?:template\s*<([^>]+)>)?\s*(?:class|struct)\s+(?:__attribute__\([^)]+\)\s+)?(\w+)(?:\s*:\s*([^{]+))?\s*\{',
            re.MULTILINE
        )
        
        self.method_pattern = re.compile(
            r'^\s*(?:(public|private|protected)\s*:)?\s*'
            r'(?:(virtual|static|inline)\s+)*'
            r'(?:(explicit|constexpr)\s+)*'
            r'([^(;]+?)\s*'
            r'(\w+)\s*\(([^)]*)\)\s*'
            r'(?:const\s+)?(?:override\s+)?(?:final\s+)?'
            r'(?:=\s*0\s*)?(?:=\s*default\s*)?(?:=\s*delete\s*)?'
            r'(?:\{[^}]*\}|;)',
            re.MULTILINE
        )
        
        self.member_pattern = re.compile(
            r'^\s*(?:(public|private|protected)\s*:)?\s*'
            r'(?:(static|mutable|const)\s+)*'
            r'([^;=]+?)\s+(\w+)\s*'
            r'(?:=\s*([^;]+))?\s*;',
            re.MULTILINE
        )
        
        self.include_pattern = re.compile(r'^\s*#include\s*[<"]([^>"]+)[>"]', re.MULTILINE)
        self.namespace_pattern = re.compile(r'^\s*namespace\s+(\w+)\s*\{', re.MULTILINE)
        self.using_pattern = re.compile(r'^\s*using\s+(?:namespace\s+)?([^;]+);', re.MULTILINE)

    def parse_directory(self, directory: str) -> None:
        """Parse tous les fichiers C++ dans un répertoire"""
        cpp_extensions = {'.cpp', '.cxx', '.cc', '.c++', '.hpp', '.hxx', '.h', '.hh'}
        
        for root, dirs, files in os.walk(directory):
            # Ignorer les répertoires de build courants
            dirs[:] = [d for d in dirs if d not in {'build', 'cmake-build-debug', 'cmake-build-release', '.git'}]
            
            for file in files:
                if Path(file).suffix.lower() in cpp_extensions:
                    file_path = os.path.join(root, file)
                    
                    # Vérifier si le fichier doit être exclu
                    if self.should_exclude_file(file_path):
                        continue
                        
                    try:
                        self.parse_file(file_path)
                    except Exception as e:
                        print(f"Erreur lors du parsing de {file_path}: {e}")

    def should_exclude_file(self, file_path: str) -> bool:
        """Vérifie si un fichier doit être exclu de l'analyse"""
        if not self.exclude_files:
            return False
            
        # Normaliser le chemin du fichier
        normalized_file_path = os.path.normpath(file_path)
        
        for exclude_pattern in self.exclude_files:
            exclude_pattern = os.path.normpath(exclude_pattern)
            
            # Vérification exacte du chemin
            if normalized_file_path == exclude_pattern:
                return True
                
            # Vérification du nom de fichier seulement
            if os.path.basename(normalized_file_path) == exclude_pattern:
                return True
                
            # Vérification si le fichier se termine par le pattern d'exclusion
            if normalized_file_path.endswith(exclude_pattern):
                return True
                
        return False

    def parse_file(self, file_path: str) -> None:
        """Parse un fichier C++ individuel"""
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception as e:
            print(f"Impossible de lire {file_path}: {e}")
            return

        # Supprimer les commentaires
        content = self.remove_comments(content)
        
        # Extraire les includes
        self.extract_includes(content, file_path)
        
        # Extraire les namespaces
        current_namespace = self.extract_namespace(content)
        
        # Extraire les classes
        self.extract_classes(content, file_path, current_namespace)

    def remove_comments(self, content: str) -> str:
        """Supprime les commentaires C++ du contenu"""
        # Supprimer les commentaires multi-lignes
        content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
        # Supprimer les commentaires de ligne
        content = re.sub(r'//.*$', '', content, flags=re.MULTILINE)
        return content

    def extract_includes(self, content: str, file_path: str) -> None:
        """Extrait les includes d'un fichier"""
        includes = set()
        for match in self.include_pattern.finditer(content):
            includes.add(match.group(1))
        self.includes[file_path] = includes

    def extract_namespace(self, content: str) -> str:
        """Extrait le namespace principal d'un fichier"""
        for match in self.namespace_pattern.finditer(content):
            return match.group(1)
        return ""

    def extract_classes(self, content: str, file_path: str, namespace: str) -> None:
        """Extrait les classes d'un fichier"""
        for match in self.class_pattern.finditer(content):
            template_params = []
            if match.group(1):
                template_params = [p.strip() for p in match.group(1).split(',')]
            
            class_name = match.group(2)
            inheritance = match.group(3) if match.group(3) else ""
            
            # Créer l'objet classe
            class_info = ClassInfo(
                name=class_name,
                namespace=namespace,
                is_template=bool(template_params),
                template_params=template_params,
                file_path=file_path
            )
            
            # Parser l'héritage
            if inheritance:
                self.parse_inheritance(inheritance, class_info)
            
            # Extraire le corps de la classe
            class_body = self.extract_class_body(content, match.end())
            if class_body:
                self.parse_class_body(class_body, class_info)
            
            # Ajouter à la liste des classes
            full_name = f"{namespace}::{class_name}" if namespace else class_name
            self.classes[full_name] = class_info

    def parse_inheritance(self, inheritance: str, class_info: ClassInfo) -> None:
        """Parse la chaîne d'héritage"""
        bases = [base.strip() for base in inheritance.split(',')]
        for base in bases:
            # Supprimer les modificateurs d'accès
            base = re.sub(r'^\s*(public|private|protected)\s+', '', base)
            base = base.strip()
            if base:
                class_info.base_classes.append(base)
                # Ajouter la relation d'héritage
                self.relationships.append(Relationship(
                    source=class_info.name,
                    target=base,
                    type=RelationType.INHERITANCE
                ))

    def extract_class_body(self, content: str, start_pos: int) -> Optional[str]:
        """Extrait le corps d'une classe en comptant les accolades"""
        brace_count = 0
        in_class = False
        class_start = -1
        
        for i, char in enumerate(content[start_pos:], start_pos):
            if char == '{':
                if not in_class:
                    in_class = True
                    class_start = i
                brace_count += 1
            elif char == '}':
                brace_count -= 1
                if brace_count == 0 and in_class:
                    return content[class_start:i+1]
        
        return None

    def parse_class_body(self, body: str, class_info: ClassInfo) -> None:
        """Parse le corps d'une classe"""
        current_access = AccessModifier.PRIVATE if 'class' in body else AccessModifier.PUBLIC
        
        # Diviser le corps en sections par modificateur d'accès
        sections = re.split(r'\n\s*(public|private|protected)\s*:', body)
        
        for i, section in enumerate(sections):
            if i > 0:
                # Mettre à jour le modificateur d'accès courant
                if i > 1:
                    access_modifier = sections[i-1].strip()
                    if access_modifier == 'public':
                        current_access = AccessModifier.PUBLIC
                    elif access_modifier == 'private':
                        current_access = AccessModifier.PRIVATE
                    elif access_modifier == 'protected':
                        current_access = AccessModifier.PROTECTED
            
            # Parser les méthodes
            self.parse_methods(section, class_info, current_access)
            
            # Parser les membres
            self.parse_members(section, class_info, current_access)

    def parse_methods(self, content: str, class_info: ClassInfo, access: AccessModifier) -> None:
        """Parse les méthodes d'une classe"""
        for match in self.method_pattern.finditer(content):
            method_access = access
            if match.group(1):
                if match.group(1) == 'public':
                    method_access = AccessModifier.PUBLIC
                elif match.group(1) == 'private':
                    method_access = AccessModifier.PRIVATE
                elif match.group(1) == 'protected':
                    method_access = AccessModifier.PROTECTED
            
            modifiers = match.group(2) if match.group(2) else ""
            return_type = match.group(4).strip() if match.group(4) else ""
            method_name = match.group(5)
            params_str = match.group(6) if match.group(6) else ""
            
            # Parser les paramètres
            parameters = self.parse_parameters(params_str)
            
            # Créer l'objet méthode
            method = ClassMethod(
                name=method_name,
                return_type=return_type,
                parameters=parameters,
                access=method_access,
                is_static='static' in modifiers,
                is_virtual='virtual' in modifiers,
                is_constructor=method_name == class_info.name,
                is_destructor=method_name.startswith('~')
            )
            
            class_info.methods.append(method)
            
            # Marquer comme abstraite si elle a des méthodes virtuelles pures
            if '= 0' in match.group(0):
                method.is_pure_virtual = True
                class_info.is_abstract = True

    def parse_members(self, content: str, class_info: ClassInfo, access: AccessModifier) -> None:
        """Parse les membres d'une classe"""
        for match in self.member_pattern.finditer(content):
            member_access = access
            if match.group(1):
                if match.group(1) == 'public':
                    member_access = AccessModifier.PUBLIC
                elif match.group(1) == 'private':
                    member_access = AccessModifier.PRIVATE
                elif match.group(1) == 'protected':
                    member_access = AccessModifier.PROTECTED
            
            modifiers = match.group(2) if match.group(2) else ""
            member_type = match.group(3).strip()
            member_name = match.group(4)
            default_value = match.group(5) if match.group(5) else None
            
            # Créer l'objet membre
            member = ClassMember(
                name=member_name,
                type=member_type,
                access=member_access,
                is_static='static' in modifiers,
                is_const='const' in modifiers,
                default_value=default_value
            )
            
            class_info.members.append(member)
            
            # Détecter les relations de composition/agrégation
            self.detect_member_relationships(member, class_info)

    def parse_parameters(self, params_str: str) -> List[Tuple[str, str]]:
        """Parse les paramètres d'une méthode"""
        parameters = []
        if not params_str.strip():
            return parameters
        
        params = [p.strip() for p in params_str.split(',')]
        for param in params:
            if '=' in param:
                param = param.split('=')[0].strip()
            
            parts = param.split()
            if len(parts) >= 2:
                param_type = ' '.join(parts[:-1])
                param_name = parts[-1]
                parameters.append((param_type, param_name))
        
        return parameters

    def detect_member_relationships(self, member: ClassMember, class_info: ClassInfo) -> None:
        """Détecte les relations basées sur les types des membres"""
        # Nettoyer le type
        clean_type = re.sub(r'[&*<>]', '', member.type).strip()
        clean_type = re.sub(r'\b(const|static|mutable)\b', '', clean_type).strip()
        
        # Détecter les conteneurs STL
        if any(container in clean_type for container in ['vector', 'list', 'set', 'map']):
            # Extraire le type de l'élément
            match = re.search(r'<([^>]+)>', member.type)
            if match:
                element_type = match.group(1).strip()
                if element_type in [c.split('::')[-1] for c in self.classes.keys()]:
                    self.relationships.append(Relationship(
                        source=class_info.name,
                        target=element_type,
                        type=RelationType.AGGREGATION,
                        label=member.name,
                        multiplicity="*"
                    ))
        
        # Détecter les pointeurs et références
        elif '*' in member.type or '&' in member.type:
            if clean_type in [c.split('::')[-1] for c in self.classes.keys()]:
                self.relationships.append(Relationship(
                    source=class_info.name,
                    target=clean_type,
                    type=RelationType.ASSOCIATION,
                    label=member.name
                ))
        
        # Détecter la composition directe
        elif clean_type in [c.split('::')[-1] for c in self.classes.keys()]:
            self.relationships.append(Relationship(
                source=class_info.name,
                target=clean_type,
                type=RelationType.COMPOSITION,
                label=member.name
            ))

    def analyze_dependencies(self) -> None:
        """Analyse les dépendances entre classes"""
        for file_path, includes in self.includes.items():
            for class_name, class_info in self.classes.items():
                if class_info.file_path == file_path:
                    for include in includes:
                        # Chercher les classes dans les includes
                        for other_class_name, other_class_info in self.classes.items():
                            if include in other_class_info.file_path and class_name != other_class_name:
                                class_info.dependencies.add(other_class_name)
                                
                                # Ajouter une relation de dépendance si pas déjà présente
                                existing_relations = [r for r in self.relationships 
                                                    if r.source == class_info.name and r.target == other_class_info.name]
                                if not existing_relations:
                                    self.relationships.append(Relationship(
                                        source=class_info.name,
                                        target=other_class_info.name,
                                        type=RelationType.DEPENDENCY
                                    ))

    def remove_duplicate_relationships(self) -> None:
        """Supprime les relations en double"""
        seen = set()
        unique_relationships = []
        
        for rel in self.relationships:
            key = (rel.source, rel.target, rel.type)
            if key not in seen:
                seen.add(key)
                unique_relationships.append(rel)
        
        self.relationships = unique_relationships


class PlantUMLGenerator:
    def __init__(self, parser: CppParser, include_private: bool = True, max_classes: Optional[int] = None, group_by_namespace: bool = True):
        self.parser = parser
        self.include_private = include_private
        self.max_classes = max_classes
        self.group_by_namespace = group_by_namespace
        self.output = []

    def sanitize_name(self, name: str) -> str:
        """Sanitise un nom pour PlantUML"""
        # Remplacer les caractères problématiques
        name = re.sub(r'[<>:,\s]', '', name)
        name = re.sub(r'[^\w]', '_', name)
        return name

    def sanitize_type(self, type_str: str) -> str:
        """Sanitise un type pour PlantUML"""
        # Simplifier les types complexes
        type_str = re.sub(r'std::', '', type_str)
        type_str = re.sub(r'<[^>]*>', '', type_str)
        type_str = re.sub(r'[&*]', '', type_str)
        type_str = re.sub(r'\s+', ' ', type_str.strip())
        return type_str

    def generate(self) -> str:
        """Génère le code PlantUML"""
        self.output = []
        
        # En-tête avec configuration pour un meilleur espacement
        self.output.append("@startuml")
        self.output.append("!theme plain")
        self.output.append("")
        self.output.append("' Configuration pour améliorer la lisibilité et l'espacement")
        self.output.append("skinparam linetype ortho")
        self.output.append("skinparam ranksep 60")
        self.output.append("skinparam nodesep 40")
        self.output.append("skinparam minClassWidth 150")
        self.output.append("skinparam classAttributeIconSize 0")
        self.output.append("skinparam classFontStyle bold")
        self.output.append("skinparam classFontSize 12")
        self.output.append("skinparam packageStyle rectangle")
        self.output.append("skinparam packageFontStyle bold")
        self.output.append("skinparam packageFontSize 14")
        self.output.append("")
        self.output.append("' Style des classes")
        self.output.append("skinparam class {")
        self.output.append("    BackgroundColor White")
        self.output.append("    BorderColor Black")
        self.output.append("    BorderThickness 1")
        self.output.append("    ArrowColor Black")
        self.output.append("    FontSize 10")
        self.output.append("}")
        self.output.append("")
        self.output.append("' Style des packages/namespaces")
        self.output.append("skinparam package {")
        self.output.append("    BackgroundColor LightGray")
        self.output.append("    BorderColor Black")
        self.output.append("    FontStyle bold")
        self.output.append("}")
        self.output.append("")
        self.output.append("' Style des relations")
        self.output.append("skinparam arrow {")
        self.output.append("    Color Black")
        self.output.append("    Thickness 1")
        self.output.append("}")
        self.output.append("")
        self.output.append("' Légende des symboles d'accès")
        self.output.append("' + : public")
        self.output.append("' - : private") 
        self.output.append("' # : protected")
        self.output.append("")
        self.output.append("' Configuration de la disposition")
        self.output.append("!define SPACING_LARGE 100")
        self.output.append("!define SPACING_MEDIUM 50")
        self.output.append("!define SPACING_SMALL 25")
        self.output.append("")
        
        # Limiter le nombre de classes si spécifié
        classes_to_process = list(self.parser.classes.items())
        if self.max_classes:
            classes_to_process = classes_to_process[:self.max_classes]
            if len(classes_to_process) < len(self.parser.classes):
                self.output.append(f"' Affichage limité à {self.max_classes} classes sur {len(self.parser.classes)} trouvées")
                self.output.append("")
        
        if self.group_by_namespace:
            # Organiser les classes par namespace
            classes_by_namespace = {}
            for class_name, class_info in classes_to_process:
                namespace = class_info.namespace or "Global"
                if namespace not in classes_by_namespace:
                    classes_by_namespace[namespace] = []
                classes_by_namespace[namespace].append(class_info)
            
            # Générer les classes organisées par namespace
            for namespace, classes in classes_by_namespace.items():
                if len(classes_by_namespace) > 1:  # Seulement si on a plusieurs namespaces
                    if namespace == "Global":
                        self.output.append("' === Classes globales ===")
                        self.output.append("")
                    else:
                        self.output.append(f"package \"{namespace}\" {{")
                        self.output.append("")
                
                for class_info in classes:
                    self.generate_class(class_info)
                    self.output.append("")
                    
                if len(classes_by_namespace) > 1 and namespace != "Global":
                    self.output.append("}")
                    self.output.append("")
        else:
            # Générer toutes les classes sans groupement
            self.output.append("' === Toutes les classes ===")
            self.output.append("")
            for class_name, class_info in classes_to_process:
                self.generate_class(class_info)
                self.output.append("")
        
        # Ajouter des directives d'espacement
        self.output.append("' Directives d'espacement pour améliorer la lisibilité")
        self.output.append("!pragma layout smetana")
        self.output.append("")
        
        # Générer les relations (uniquement si les deux classes existent)
        self.output.append("' Relations entre les classes")
        for relationship in self.parser.relationships:
            self.generate_relationship(relationship)
        
        # Directives finales pour améliorer la disposition
        self.output.append("")
        self.output.append("' Directives pour améliorer l'espacement final")
        self.output.append("!pragma layout smetana")
        self.output.append("")
        
        # Pied de page
        self.output.append("@enduml")
        
        return '\n'.join(self.output)

    def generate_class(self, class_info: ClassInfo) -> None:
        """Génère une classe PlantUML"""
        class_type = "abstract class" if class_info.is_abstract else "class"
        class_name = self.sanitize_name(class_info.name)
        
        # Gérer les templates différemment pour PlantUML
        if class_info.is_template and class_info.template_params:
            template_str = f"<<T>>"
            class_name = f"{class_name}{template_str}"
        
        # Ajouter un commentaire descriptif pour la classe
        self.output.append(f"' Classe: {class_info.name}")
        if class_info.file_path:
            self.output.append(f"' Fichier: {class_info.file_path}")
        
        self.output.append(f"{class_type} {class_name} {{")
        
        # Filtrer les membres selon l'option include_private
        filtered_members = [
            member for member in class_info.members
            if self.include_private or member.access != AccessModifier.PRIVATE
        ]
        
        # Filtrer les méthodes selon l'option include_private
        filtered_methods = [
            method for method in class_info.methods
            if self.include_private or method.access != AccessModifier.PRIVATE
        ]
        
        # Organiser les membres par type d'accès pour une meilleure lisibilité
        public_members = [m for m in filtered_members if m.access == AccessModifier.PUBLIC]
        protected_members = [m for m in filtered_members if m.access == AccessModifier.PROTECTED]
        private_members = [m for m in filtered_members if m.access == AccessModifier.PRIVATE]
        
        # Organiser les méthodes par type d'accès
        public_methods = [m for m in filtered_methods if m.access == AccessModifier.PUBLIC]
        protected_methods = [m for m in filtered_methods if m.access == AccessModifier.PROTECTED]
        private_methods = [m for m in filtered_methods if m.access == AccessModifier.PRIVATE]
        
        # Générer les membres publics
        if public_members:
            self.output.append("  ' === Attributs publics ===")
            for member in public_members:
                self.generate_member(member)
            self.output.append("")
        
        # Générer les membres protégés
        if protected_members:
            self.output.append("  ' === Attributs protégés ===")
            for member in protected_members:
                self.generate_member(member)
            self.output.append("")
        
        # Générer les membres privés
        if private_members:
            self.output.append("  ' === Attributs privés ===")
            for member in private_members:
                self.generate_member(member)
            self.output.append("")
        
        # Séparateur entre attributs et méthodes
        if (public_members or protected_members or private_members) and (public_methods or protected_methods or private_methods):
            self.output.append("  __")
            self.output.append("")
        
        # Générer les méthodes publiques
        if public_methods:
            self.output.append("  ' === Méthodes publiques ===")
            for method in public_methods:
                self.generate_method(method)
            self.output.append("")
        
        # Générer les méthodes protégées
        if protected_methods:
            self.output.append("  ' === Méthodes protégées ===")
            for method in protected_methods:
                self.generate_method(method)
            self.output.append("")
        
        # Générer les méthodes privées
        if private_methods:
            self.output.append("  ' === Méthodes privées ===")
            for method in private_methods:
                self.generate_method(method)
            self.output.append("")
        
        self.output.append("}")

    def generate_member(self, member: ClassMember) -> None:
        """Génère un membre de classe"""
        access_symbol = member.access.value
        modifiers = []
        
        if member.is_static:
            modifiers.append("{static}")
        if member.is_const:
            modifiers.append("{readonly}")
        
        modifier_str = " ".join(modifiers)
        sanitized_type = self.sanitize_type(member.type)
        sanitized_name = member.name  # Garder le nom original pour plus de lisibilité
        
        # Format amélioré : access nom : type modifiers
        line = f"  {access_symbol} **{sanitized_name}** : {sanitized_type}"
        if modifier_str:
            line += f" {modifier_str}"
        if member.default_value:
            line += f" = {member.default_value}"
        
        self.output.append(line)

    def generate_method(self, method: ClassMethod) -> None:
        """Génère une méthode de classe"""
        access_symbol = method.access.value
        modifiers = []
        
        if method.is_static:
            modifiers.append("{static}")
        if method.is_virtual or method.is_pure_virtual:
            modifiers.append("{abstract}")
        
        # Paramètres avec noms et types
        params = []
        for ptype, pname in method.parameters:
            sanitized_type = self.sanitize_type(ptype)
            if pname:
                params.append(f"{pname}: {sanitized_type}")
            else:
                params.append(sanitized_type)
        params_str = ", ".join(params)
        
        # Nom de la méthode (garder le nom original)
        method_name = method.name
        method_prefix = ""
        if method.is_constructor:
            method_prefix = "<<create>> "
        elif method.is_destructor:
            method_prefix = "<<destroy>> "
        
        sanitized_return_type = self.sanitize_type(method.return_type)
        modifier_str = " ".join(modifiers)
        
        # Format amélioré : access nom(paramètres) : type_retour modifiers
        if method.is_constructor or method.is_destructor:
            line = f"  {access_symbol} {method_prefix}**{method_name}**({params_str})"
        else:
            line = f"  {access_symbol} **{method_name}**({params_str}) : {sanitized_return_type}"
            
        if modifier_str:
            line += f" {modifier_str}"
        
        self.output.append(line)

    def generate_relationship(self, relationship: Relationship) -> None:
        """Génère une relation PlantUML"""
        source = self.sanitize_name(relationship.source)
        target = self.sanitize_name(relationship.target)
        arrow = relationship.type.value
        
        # Éviter les relations vers des types primitifs ou inconnus
        if target.lower() in ['int', 'float', 'double', 'char', 'bool', 'void', 'string', 'size_t']:
            return
        
        # Vérifier que les classes existent
        source_exists = any(self.sanitize_name(cls) == source for cls in self.parser.classes.keys())
        target_exists = any(self.sanitize_name(cls) == target for cls in self.parser.classes.keys())
        
        if not source_exists or not target_exists:
            return
        
        # Ajouter un commentaire pour identifier le type de relation
        relation_names = {
            RelationType.INHERITANCE: "Héritage",
            RelationType.COMPOSITION: "Composition",
            RelationType.AGGREGATION: "Agrégation",
            RelationType.ASSOCIATION: "Association",
            RelationType.DEPENDENCY: "Dépendance",
            RelationType.IMPLEMENTATION: "Implémentation"
        }
        relation_name = relation_names.get(relationship.type, "Relation")
        
        line = f"{source} {arrow} {target}"
        if relationship.label:
            sanitized_label = self.sanitize_name(relationship.label)
            line += f" : {sanitized_label}"
        if relationship.multiplicity:
            line += f" \"({relationship.multiplicity})\""
        
        # Ajouter un commentaire descriptif
        self.output.append(f"' {relation_name}: {source} -> {target}")
        self.output.append(line)


def main():
    """Fonction principale"""
    parser = argparse.ArgumentParser(
        description="Génère un diagramme de classes PlantUML à partir d'un projet C++ CMake"
    )
    
    parser.add_argument(
        "input_dir",
        help="Répertoire contenant le code source C++ à analyser"
    )
    
    parser.add_argument(
        "output_file",
        help="Fichier de sortie PlantUML (.puml)"
    )
    
    parser.add_argument(
        "--exclude",
        nargs="*",
        default=[],
        help="Répertoires à exclure de l'analyse"
    )
    
    parser.add_argument(
        "--exclude-files",
        nargs="*",
        default=[],
        help="Fichiers spécifiques à exclure de l'analyse (chemins relatifs ou absolus)"
    )
    
    parser.add_argument(
        "--include-private",
        action="store_true",
        help="Inclure les membres privés dans le diagramme"
    )
    
    parser.add_argument(
        "--namespace",
        help="Filtrer par namespace spécifique"
    )
    
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Affichage détaillé"
    )
    
    parser.add_argument(
        "--max-classes",
        type=int,
        help="Nombre maximum de classes à afficher (pour éviter les diagrammes trop complexes)"
    )
    
    parser.add_argument(
        "--group-by-namespace",
        action="store_true",
        default=True,
        help="Grouper les classes par namespace dans des packages"
    )
    
    args = parser.parse_args()
    
    # Vérifier que le répertoire d'entrée existe
    if not os.path.exists(args.input_dir):
        print(f"Erreur: Le répertoire {args.input_dir} n'existe pas")
        return 1
    
    # Créer le répertoire de sortie si nécessaire
    output_dir = os.path.dirname(args.output_file)
    if output_dir and not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    if args.verbose:
        print(f"Analyse du répertoire: {args.input_dir}")
        print(f"Fichier de sortie: {args.output_file}")
    
    try:
        # Créer le parser et analyser
        cpp_parser = CppParser(exclude_files=args.exclude_files)
        cpp_parser.parse_directory(args.input_dir)
        
        if args.verbose:
            print(f"Classes trouvées: {len(cpp_parser.classes)}")
            for class_name in cpp_parser.classes:
                print(f"  - {class_name}")
        
        # Analyser les dépendances
        cpp_parser.analyze_dependencies()
        
        # Supprimer les relations en double
        cpp_parser.remove_duplicate_relationships()
        
        # Filtrer par namespace si spécifié
        if args.namespace:
            filtered_classes = {
                name: cls for name, cls in cpp_parser.classes.items()
                if cls.namespace == args.namespace
            }
            cpp_parser.classes = filtered_classes
        
        if args.verbose:
            print(f"Relations trouvées: {len(cpp_parser.relationships)}")
        
        # Générer le PlantUML
        generator = PlantUMLGenerator(
            cpp_parser, 
            include_private=args.include_private,
            max_classes=args.max_classes,
            group_by_namespace=args.group_by_namespace
        )
        plantuml_content = generator.generate()
        
        # Écrire le fichier de sortie
        with open(args.output_file, 'w', encoding='utf-8') as f:
            f.write(plantuml_content)
        
        print(f"Diagramme généré avec succès: {args.output_file}")
        
        if args.verbose:
            print(f"Lignes générées: {len(plantuml_content.split(chr(10)))}")
        
        return 0
        
    except Exception as e:
        print(f"Erreur lors de la génération: {e}")
        if args.verbose:
            import traceback
            traceback.print_exc()
        return 1


if __name__ == "__main__":
    exit(main())